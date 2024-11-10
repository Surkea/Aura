#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitValues()
{
	if (const UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
		OnManaChanged.Broadcast(AS->GetMana());
		OnMaxManaChanged.Broadcast(AS->GetMaxMana());
	}
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	if (const UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute())
		.AddUObject(this, &UOverlayWidgetController::HealthChanged);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute())
		.AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetManaAttribute())
		.AddUObject(this, &UOverlayWidgetController::ManaChanged);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxManaAttribute())
		.AddUObject(this, &UOverlayWidgetController::MaxManaChanged);
	}
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);

}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}
