#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAbilityTypes.h"
#include "AbilitySystem/AuraAttributeSet.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	//Resistances
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		//Resistances
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);

		//Map tags to capture definitions
		const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
		TagsToCaptureDefs.Add(AuraTags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(AuraTags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(AuraTags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefs.Add(AuraTags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDefs.Add(AuraTags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
		TagsToCaptureDefs.Add(AuraTags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
		//Resistances
		TagsToCaptureDefs.Add(AuraTags.Attributes_Resistances_Physical, PhysicalResistanceDef);
		TagsToCaptureDefs.Add(AuraTags.Attributes_Resistances_Fire, FireResistanceDef);
		TagsToCaptureDefs.Add(AuraTags.Attributes_Resistances_Lightning, LightningResistanceDef);
		TagsToCaptureDefs.Add(AuraTags.Attributes_Resistances_Arcane, ArcaneResistanceDef);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics* AuraDamageStaticsPtr = nullptr;
	if (!AuraDamageStaticsPtr)
	{
		AuraDamageStaticsPtr = new AuraDamageStatics();
	}
	return *AuraDamageStaticsPtr;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	//Resistances
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//Capture relative attributes
	float TarArmor = 0.f;
	float SrcArmorPenetration = 0.f;
	float TarBlockChance = 0.f;
	float SrcCriticalHitChance = 0.f;
	float SrcCriticalHitDamage = 0.f;
	float TarCriticalHitResistance = 0.f;

	ExecutionParams.
		AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TarArmor);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef,
	                                                           EvaluationParameters, SrcArmorPenetration);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters,
	                                                           TarBlockChance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef,
	                                                           EvaluationParameters, SrcCriticalHitChance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef,
	                                                           EvaluationParameters, SrcCriticalHitDamage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef,
	                                                           EvaluationParameters, TarCriticalHitResistance);

	TarArmor = FMath::Clamp(TarArmor, 0.f, 100.f);
	SrcArmorPenetration = FMath::Clamp(SrcArmorPenetration, 0.f, 100.f);
	TarBlockChance = FMath::Max(TarBlockChance, 0.f);
	SrcCriticalHitChance = FMath::Max(SrcCriticalHitChance, 0.f);
	SrcCriticalHitDamage = FMath::Max(SrcCriticalHitDamage, 0.f);
	TarCriticalHitResistance = FMath::Max(TarCriticalHitResistance, 0.f);

	//Get origin damage value
	float Damage = 0.f;
	for (auto& DamageTypeTagToResistance : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = DamageTypeTagToResistance.Key;
		const FGameplayTag ResistanceTag = DamageTypeTagToResistance.Value;
		const FGameplayEffectAttributeCaptureDefinition* ResistanceCaptureDef = DamageStatics().TagsToCaptureDefs.Find(
			ResistanceTag);
		float ResistanceValue = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(*ResistanceCaptureDef, EvaluationParameters,
		                                                           ResistanceValue);
		ResistanceValue = FMath::Clamp(ResistanceValue, 0.f, 100.f);
		
		const float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTagToResistance.Key) * (100 - ResistanceValue) / 100;
		Damage += DamageTypeValue;
	}

	// Get the context
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	
	//Block check
	const bool bIsBlocked = FMath::RandRange(1, 100) <= TarBlockChance;
	if (bIsBlocked)
	{
		Damage /= 2.f;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Blocked"));
	}
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bIsBlocked);


	//Armor and Penetration
	const float EffectiveArmor = TarArmor * (100 - SrcArmorPenetration) / 100;
	Damage *= (100 - EffectiveArmor) / 100;

	//Critical Hit
	const float EffectiveCriticalHitChance = FMath::Clamp(SrcCriticalHitChance - TarCriticalHitResistance * 0.2, 0.f,
	                                                      100.f);
	const bool bIsCritical = (FMath::RandRange(1, 100) <= EffectiveCriticalHitChance) && !bIsBlocked;
	if (bIsCritical)
	{
		Damage = Damage * 2 + SrcCriticalHitDamage;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Critical Hit"));
	}
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bIsCritical);


	//Apply damage
	const FGameplayModifierEvaluatedData DamageData(UAuraAttributeSet::GetIncomingDamageAttribute(),
	                                                EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(DamageData);
}
