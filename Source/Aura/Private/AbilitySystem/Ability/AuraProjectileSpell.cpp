#include "AbilitySystem/Ability/AuraProjectileSpell.h"

#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (const bool bIsServer = HasAuthority(&ActivationInfo))
	{

		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
		{
			const FVector SpawnLocation = CombatInterface->GetWeaponSocketLocation();
			const FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, SpawnLocation);

			AActor* OwningActor = GetOwningActorFromActorInfo();
			AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, OwningActor,
															Cast<APawn>(OwningActor),
															ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			Projectile->FinishSpawning(SpawnTransform);
		}
		
	}
}
