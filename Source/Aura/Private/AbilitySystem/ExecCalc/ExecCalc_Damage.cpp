#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics AuraDamageStatics;
	return AuraDamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
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
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Meta_Damage, true);

	//Block check
	if (bool bIsBlocked = FMath::RandRange(1, 100) <= TarBlockChance)
	{
		Damage /= 2.f;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Blocked"));
	}

	//Armor and Penetration
	const float EffectiveArmor = TarArmor * (100 - SrcArmorPenetration) / 100;
	Damage *= (100 - EffectiveArmor) / 100;

	//Critical Hit
	const float EffectiveCriticalHitChance = FMath::Clamp(SrcCriticalHitChance - TarCriticalHitResistance * 0.2, 0.f,
	                                                      100.f);
	if (FMath::RandRange(1, 100) <= EffectiveCriticalHitChance)
	{
		Damage = Damage * 2 + SrcCriticalHitDamage;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Critical Hit"));
	}

	//Apply damage
	const FGameplayModifierEvaluatedData DamageData(UAuraAttributeSet::GetIncomingDamageAttribute(),
	                                                EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(DamageData);
}
