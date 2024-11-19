#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GetTargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataDelegate, const FGameplayAbilityTargetDataHandle&, DataHandle);
/**
 * 
 */
UCLASS()
class AURA_API UGetTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
		meta=(DisplayName = "GetTargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf="OwningAbility",
			BlueprintInternalUseOnly = "TRUE"))
	static UGetTargetDataUnderMouse* CreateProxy(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataDelegate OnGetMouseTargetData;

private:

	virtual void Activate() override;

	void SendMouseCursorData() const;

	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag) const;
};
