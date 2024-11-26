#include "Player/AuraPlayerController.h"

#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
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

void AAuraPlayerController::ShowDamageNumber_Implementation(float Damage, ACharacter* Target, bool bIsBlocked, bool bIsCritical)
{
	if (IsValid(Target) && IsLocalController()) BP_ShowDamageNumber(Damage, Target, bIsBlocked, bIsCritical);
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
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
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
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHitResult);
	if (!CursorHitResult.bBlockingHit) return;

	LastActor = CurActor;
	CurActor = Cast<IEnemyInterface>(CursorHitResult.GetActor());

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
		const FVector Direction_1 = PathSpline->FindDirectionClosestToWorldLocation(
			ClosestPoint, ESplineCoordinateSpace::World);
		const FVector Direction_2 = (ClosestPoint - ControlledPawn->GetActorLocation()).GetSafeNormal();
		const FVector Direction = (Direction_1 + Direction_2).GetSafeNormal();
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

	if (GetAuraAbilitySystemComponent() == nullptr) return;
	GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);

	if (!bAimingTarget && !bShiftKeyDown)
	{
		/*if (FollowTime <= ShortPressTS)
	{*/
		const APawn* ControlledPawn = GetPawn<APawn>();
		if (const UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
			this, ControlledPawn->GetActorLocation(), CachedDestination))
		{
			PathSpline->ClearSplinePoints();
			for (const FVector& Point : NavPath->PathPoints)
			{
				PathSpline->AddSplinePoint(Point, ESplineCoordinateSpace::World);
			}
			if (NavPath->PathPoints.Num() > 0) CachedDestination = NavPath->PathPoints.Last();
			bAutoRunning = true;
		}
		/*}*/
		FollowTime = 0.f;
		bAimingTarget = false;
	}
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
	if (bAimingTarget || bShiftKeyDown) //瞄准目标释放技能或自由施法
	{
		if (GetAuraAbilitySystemComponent() == nullptr) return;
		GetAuraAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
	}
	else //点击地面，如果判断为长按状态，自动寻路并移动
	{
		FollowTime += GetWorld()->GetTimeSeconds();

		if (!CursorHitResult.bBlockingHit) return;
		CachedDestination = CursorHitResult.ImpactPoint;

		if (APawn* ControlledPawn = GetPawn<APawn>())
		{
			const FVector Direction = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(Direction);
		}
	}
}
