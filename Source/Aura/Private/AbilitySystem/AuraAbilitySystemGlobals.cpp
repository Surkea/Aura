#include "AbilitySystem/AuraAbilitySystemGlobals.h"

#include "AbilitySystem/AuraAbilityTypes.h"

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAuraGameplayEffectContext();
}
