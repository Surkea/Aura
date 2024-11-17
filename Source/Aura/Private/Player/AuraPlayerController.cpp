#include "Player/AuraPlayerController.h"

#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
}


void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	checkf(InputConfig, TEXT("InputConfig is not set in %s"), *GetName());

	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityTagPressed,
	                                       &ThisClass::AbilityTagReleased, &ThisClass::AbilityTagHeld);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	bAutoRunning = false;

	const FVector2d InputAxisVector = InputActionValue.Get<FVector2d>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, 0.7 * InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, 0.7 * InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (!HitResult.bBlockingHit) return;

	LastActor = CurActor;
	CurActor = Cast<IEnemyInterface>(HitResult.GetActor());

	if (LastActor != CurActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (CurActor) CurActor->HighlightActor();
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		const FVector ClosestPoint = PathSpline->FindLocationClosestToWorldLocation(
			ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = PathSpline->FindDirectionClosestToWorldLocation(
			ClosestPoint, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);
		if ((ControlledPawn->GetActorLocation() - CachedDestination).Length() <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

inline UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAbilitySystemComponent()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::AbilityTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().Input_LMB))
	{
		bAimingTarget = CurActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().Input_LMB))
	{
		if (GetAuraAbilitySystemComponent() == nullptr) return;
		GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
		return;
	}

	if (bAimingTarget) //瞄准目标响应release
	{
		if (GetAuraAbilitySystemComponent() == nullptr) return;
		GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
		return;
	}

	if (FollowTime <= ShortPressTS)
	{
		const APawn* ControlledPawn = GetPawn<APawn>();
		if (const UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
			this, ControlledPawn->GetActorLocation(), CachedDestination))
		{
			PathSpline->ClearSplinePoints();
			for (const FVector& Point : NavPath->PathPoints)
			{
				PathSpline->AddSplinePoint(Point, ESplineCoordinateSpace::World);
				DrawDebugSphere(GetWorld(), Point, 5.f, 4, FColor::Green, false, 1.f);
			}
			CachedDestination = NavPath->PathPoints.Last();
			bAutoRunning = true;
		}
	}
	FollowTime = 0.f;
	bAimingTarget = false;
}

void AAuraPlayerController::AbilityTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().Input_LMB))
	{
		if (GetAuraAbilitySystemComponent() == nullptr) return;
		GetAuraAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
		return;
	}
	//按下左键
	if (bAimingTarget) //瞄准目标释放技能
	{
		if (GetAuraAbilitySystemComponent() == nullptr) return;
		GetAuraAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
	}
	else //点击地面，如果判断为长按状态，自动寻路并移动
	{
		FollowTime += GetWorld()->GetTimeSeconds();

		FHitResult HitResult;
		GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
		if (!HitResult.bBlockingHit) return;
		CachedDestination = HitResult.ImpactPoint;

		if (APawn* ControlledPawn = GetPawn<APawn>())
		{
			const FVector Direction = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(Direction);
		}
	}
}
