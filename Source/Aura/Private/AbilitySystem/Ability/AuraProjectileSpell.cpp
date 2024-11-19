#include "AbilitySystem/Ability/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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
	if (const bool bIsServer = HasAuthority(&CurrentActivationInfo))
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
		{
			const FVector SpawnLocation = CombatInterface->GetWeaponSocketLocation();
			FRotator SpawnRotation = (TargetProjectileLocation - SpawnLocation).Rotation();
			SpawnRotation.Pitch = 0.f;
			const FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation);

			AActor* OwningActor = GetOwningActorFromActorInfo();
			AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
				ProjectileClass, SpawnTransform, OwningActor,
				Cast<APawn>(OwningActor),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			if (const UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
				GetAvatarActorFromActorInfo()))
			{
				Projectile->EffectSpecHandle = ASC->MakeOutgoingSpec(ProjectileEffectClass, GetAbilityLevel(),
				                                                     ASC->MakeEffectContext());
			}
			Projectile->FinishSpawning(SpawnTransform);
		}
	}
}
