// Ko


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::OnDie_Implementation()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	MulticastHandleDeath_Implementation();
}

bool AAuraCharacterBase::GetIsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatarActor_Implementation()
{
	return this;
}


void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Weapon->AddImpulse(FVector(0.f, 0.f, 200.f), NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	GetMesh()->AddImpulse(FVector(0.f, 0.f, 4000.f), NAME_None, true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Dissolve();
	bDead = true;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	if (MontageTag.MatchesTag(AuraTags.Montage_Attack_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponSocketName);
	}
	if(MontageTag.MatchesTag(AuraTags.Montage_Attack_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if(MontageTag.MatchesTag(AuraTags.Montage_Attack_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	return FVector::ZeroVector;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
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


UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodFX;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByDataTag_Implementation(const FGameplayTag& MontageDataTag)
{
	for (FTaggedMontage Montage : AttackMontages)
	{
		if (Montage.MontageDataTag.MatchesTag(MontageDataTag))
		{
			return Montage;
		}
	}
	return FTaggedMontage();
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* InstanceDynamic = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, InstanceDynamic);
		StartDissolveTimeline(InstanceDynamic);
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* InstanceDynamic = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, InstanceDynamic);
		StartWeaponDissolveTimeline(InstanceDynamic);
	}
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}
