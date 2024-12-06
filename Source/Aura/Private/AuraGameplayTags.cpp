#include "AuraGameplayTags.h"

#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	//Primary Attributes
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Strength"));
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Intelligence"));
	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Resilience"));
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Vigor"));

	//Secondary Attributes
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"));
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ArmorPenetration"));
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.BlockChance"));
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitChance"));
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitDamage"));
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitResistance"));
	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.HealthRegeneration"));
	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ManaRegeneration"));
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxHealth"));
	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxMana"));\

	//Resistances
	GameplayTags.Attributes_Resistances_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistances.Physical"));
	GameplayTags.Attributes_Resistances_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistances.Fire"));
	GameplayTags.Attributes_Resistances_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistances.Lightning"));
	GameplayTags.Attributes_Resistances_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistances.Arcane"));

	//Input
	GameplayTags.Input_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.LMB"));
	GameplayTags.Input_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.RMB"));
	GameplayTags.Input_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.1"));
	GameplayTags.Input_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.2"));
	GameplayTags.Input_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.3"));
	GameplayTags.Input_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.4"));

	//Meta
	GameplayTags.Meta_Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Meta.Damage.Physical"));
	GameplayTags.Meta_Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Meta.Damage.Fire"));
	GameplayTags.Meta_Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Meta.Damage.Lightning"));
	GameplayTags.Meta_Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Meta.Damage.Arcane"));

	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Meta_Damage_Physical, GameplayTags.Attributes_Resistances_Physical);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Meta_Damage_Fire, GameplayTags.Attributes_Resistances_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Meta_Damage_Lightning, GameplayTags.Attributes_Resistances_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Meta_Damage_Arcane, GameplayTags.Attributes_Resistances_Arcane);

	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Attack"));

	//Effect
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Effects.HitReact"));
	GameplayTags.Montage_Attack_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.Weapon"));
	GameplayTags.Montage_Attack_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.LeftHand"));
	GameplayTags.Montage_Attack_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.RightHand"));

	GameplayTags.TaggedMontage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("TaggedMontage.Attack.1"));
	GameplayTags.TaggedMontage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("TaggedMontage.Attack.2"));
	GameplayTags.TaggedMontage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("TaggedMontage.Attack.3"));
	GameplayTags.TaggedMontage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("TaggedMontage.Attack.4"));
}
