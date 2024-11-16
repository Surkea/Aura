// Ko


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> Effect, const float Level) const
{
	FGameplayEffectContextHandle Ctx = GetAbilitySystemComponent()->MakeEffectContext();
	Ctx.AddSourceObject(this);
	const FGameplayEffectSpecHandle Spec = GetAbilitySystemComponent()->MakeOutgoingSpec(Effect, Level, Ctx);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	check(DefaultPrimaryAttributes);
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	check(DefaultSecondaryAttributes);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	check(DefaultVitalAttributes);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities() const
{
	if (!HasAuthority()) return;

	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	ASC->AddCharacterAbilities(StartAbilities);
}


void AAuraCharacterBase::InitAbilityActorInfo()
{
}
