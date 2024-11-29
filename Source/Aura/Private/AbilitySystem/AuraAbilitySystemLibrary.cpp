#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystem/AuraAbilityTypes.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams Params(PC, PS, ASC, AS);
			return HUD->GetOverlayWidgetController(Params);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams Params(PC, PS, ASC, AS);
			return HUD->GetAttributeMenuWidgetController(Params);
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitDefaultAttributes(const UObject* WorldContextObject,
                                                      const ECharacterClass CharacterClass,
                                                      UAbilitySystemComponent* ASC, const float Level)
{
	if (const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		const FCharacterClassDefaultInfo DefaultInfo = GetCharacterClassInfo(WorldContextObject)->GetClassDefaultInfo(
			CharacterClass);
		const AActor* AvatarActor = ASC->GetAvatarActor();
		FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
		Ctx.AddSourceObject(AvatarActor);
		const FGameplayEffectSpecHandle PrimarySpec = ASC->MakeOutgoingSpec(
			DefaultInfo.PrimaryAttribute, Level, Ctx);
		const FGameplayEffectSpecHandle VitalSpec = ASC->MakeOutgoingSpec(
			GameMode->CharacterClassInfo->VitalAttribute, Level, Ctx);
		const FGameplayEffectSpecHandle SecondarySpec = ASC->MakeOutgoingSpec(
			GameMode->CharacterClassInfo->SecondaryAttribute, Level, Ctx);

		ASC->ApplyGameplayEffectSpecToSelf(*PrimarySpec.Data.Get());
		ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpec.Data.Get());
		ASC->ApplyGameplayEffectSpecToSelf(*VitalSpec.Data.Get());
	}
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, const ECharacterClass CharacterClass)
{
	const UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	ICombatInterface* Interface = Cast<ICombatInterface>(ASC->GetAvatarActor());
	const int32 Level = Interface->GetPlayerLevel();
	for (const TSubclassOf<UGameplayAbility> GA : ClassInfo->CommonAbilities)
	{
		ASC->GiveAbility(FGameplayAbilitySpec(GA, Level));
	}
	const FCharacterClassDefaultInfo DefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);
	for (const TSubclassOf<UGameplayAbility> GA : DefaultInfo.DefaultAbilities)
	{
		ASC->GiveAbility(FGameplayAbilitySpec(GA, Level));
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	if (const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return GameMode->CharacterClassInfo;
	}
	return nullptr;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsBlockedHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle,
	const bool bIsBlocked)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsBlockedHit(bIsBlocked);
	}
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(
	EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bIsCritical)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(
	EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsCriticalHit(bIsCritical);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutActors, const TArray<AActor*>& IgnoreActors, const FVector& OriginLocation, float Radius)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(IgnoreActors);

	TArray<FOverlapResult> Overlaps;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, OriginLocation, FQuat::Identity, FCollisionObjectQueryParams(
			ECC_Pawn), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (const ICombatInterface* Interface = Cast<ICombatInterface>(Overlap.GetActor()))
			{
				if (Interface->Execute_GetIsDead(Overlap.GetActor()))
				{
					continue;	
				}
				{
					OutActors.AddUnique(Interface->Execute_GetAvatarActor(Overlap.GetActor()));
				}
			}
		}
	}
}
