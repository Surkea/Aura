#include "AbilitySystem/Ability/AuraSummonAbility.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector Origin = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const float DeltaRadius = SpawnRadius / NumMinions;

	const FVector RightLimit = Forward.RotateAngleAxis(SpawnRadius / 2, FVector::UpVector);
	const FVector LeftLimit = Forward.RotateAngleAxis(-SpawnRadius / 2, FVector::UpVector);

	return TArray<FVector>();
	
}
