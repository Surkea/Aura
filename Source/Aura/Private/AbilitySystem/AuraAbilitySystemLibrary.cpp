#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Game/AuraGameModeBase.h"
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
		const FCharacterClassDefaultInfo DefaultInfo = GetCharacterClassInfo(WorldContextObject)->GetClassDefaultInfo(CharacterClass);
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

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	const UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	for (const TSubclassOf<UGameplayAbility> GA : ClassInfo->CommonAbilities)
	{
		ASC->GiveAbility(FGameplayAbilitySpec(GA, 1));
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
