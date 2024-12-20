#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * Singleton containing native gameplay tags
 */

struct FAuraGameplayTags
{
	static const FAuraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	//Primary Attributes
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	//Secondary Attributes
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;

	//Resistances
	FGameplayTag Attributes_Resistances_Physical;
	FGameplayTag Attributes_Resistances_Fire;
	FGameplayTag Attributes_Resistances_Lightning;
	FGameplayTag Attributes_Resistances_Arcane;
	

	//Input
	FGameplayTag Input_LMB;
	FGameplayTag Input_RMB;
	FGameplayTag Input_1;
	FGameplayTag Input_2;
	FGameplayTag Input_3;
	FGameplayTag Input_4;

	//Meta
	FGameplayTag Meta_Damage_Physical;
	FGameplayTag Meta_Damage_Fire;
	FGameplayTag Meta_Damage_Lightning;
	FGameplayTag Meta_Damage_Arcane;

	FGameplayTag Abilities_Attack;

	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;

	//Effects
	FGameplayTag Effects_HitReact;
	FGameplayTag Montage_Attack_Weapon;
	FGameplayTag Montage_Attack_LeftHand;
	FGameplayTag Montage_Attack_RightHand;

	FGameplayTag TaggedMontage_Attack_1;
	FGameplayTag TaggedMontage_Attack_2;
	FGameplayTag TaggedMontage_Attack_3;
	FGameplayTag TaggedMontage_Attack_4;


private:
	static FAuraGameplayTags GameplayTags;
};
