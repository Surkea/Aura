#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,
	Warrior,
	Ranger
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Class Default")
	TSubclassOf<UGameplayEffect> PrimaryAttribute;

	UPROPERTY(EditDefaultsOnly, Category="Class Default")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
};
/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category="Character Class Default")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInfoMap;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Default")
	TSubclassOf<UGameplayEffect> VitalAttribute;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Default")
	TSubclassOf<UGameplayEffect> SecondaryAttribute;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Default")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UFUNCTION()
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass) const;
	
	
};
