#include "AbilitySystem/Ability/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& TargetProjectileLocation)
{
	if (const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority())
	{
		if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
		{
			const FVector SpawnLocation = CombatInterface->Execute_GetCombatSocketLocation(
				GetAvatarActorFromActorInfo(),
				FAuraGameplayTags::Get().Montage_Attack_Weapon);
			FRotator SpawnRotation = (TargetProjectileLocation - SpawnLocation).Rotation();
			//SpawnRotation.Pitch = 0.f;
			const FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation);

			AActor* OwningActor = GetOwningActorFromActorInfo();
			AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
				ProjectileClass, SpawnTransform, OwningActor,
				Cast<APawn>(OwningActor),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			if (const UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
				GetAvatarActorFromActorInfo()))
			{
				FGameplayEffectContextHandle CtxHandle = ASC->MakeEffectContext();
				CtxHandle.SetAbility(this);
				CtxHandle.AddSourceObject(Projectile);
				
				const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(),CtxHandle);
				const FAuraGameplayTags Tags = FAuraGameplayTags::Get();

				for (auto& Pair:DamageTypes)
				{
					const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Spec, Pair.Key, ScaledDamage);
				}
				
				Projectile->EffectSpecHandle = Spec;
			}
			Projectile->FinishSpawning(SpawnTransform);
		}
	}
}
