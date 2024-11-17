#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"


class USplineComponent;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();

	IEnemyInterface* LastActor;
	IEnemyInterface* CurActor;

	void AbilityTagPressed(FGameplayTag InputTag);
	void AbilityTagReleased(FGameplayTag InputTag);
	void AbilityTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	const TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();

	// Movement
	//lmb Held的时候点击的地面位置
	FVector CachedDestination = FVector::ZeroVector;
	//lmb按下到松开的持续时间
	float FollowTime = 0.f;
	//短按时间阈值ms
	float ShortPressTS = 500.f;
	//是否正点击地面后的处于寻路状态
	bool bAutoRunning = false;
	//lmb press的时候是否选中了目标
	bool bAimingTarget = false;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> PathSpline;
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 100.f;

	void AutoRun();
};
