#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations(); 

	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	int32 NumMinions = 1;

	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	float MinSpawnDistance = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	float MaxSpawnDistance = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	float SpawnRadius = 100.0f;
};
