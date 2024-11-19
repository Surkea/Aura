#include "AbilitySystem/AbilityTask/GetTargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UGetTargetDataUnderMouse* UGetTargetDataUnderMouse::CreateProxy(UGameplayAbility* OwningAbility)
{
	UGetTargetDataUnderMouse* MyObj = NewAbilityTask<UGetTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UGetTargetDataUnderMouse::Activate()
{
	if (const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		SendMouseCursorData();
	}
	else
	{
		//Listen for data on server
		FGameplayAbilitySpecHandle AbilitySpecHandle = GetAbilitySpecHandle();
		FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(AbilitySpecHandle, ActivationPredictionKey)
		                      .AddUObject(this, &UGetTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		if (const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(
			AbilitySpecHandle, ActivationPredictionKey); !bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UGetTargetDataUnderMouse::SendMouseCursorData() const
{
	FScopedPredictionWindow ScopedPredictionWindow(AbilitySystemComponent.Get());

	const APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult HitResult;
	PC->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, false, HitResult);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
	DataHandle.Add(TargetData);
	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(),
	                                                      DataHandle, FGameplayTag(),
	                                                      AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnGetMouseTargetData.Broadcast(DataHandle);
	}
}

void UGetTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data,
                                                              FGameplayTag ActivationTag) const
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnGetMouseTargetData.Broadcast(Data);
	}
}
