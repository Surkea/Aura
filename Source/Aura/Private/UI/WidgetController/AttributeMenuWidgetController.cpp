#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UAttributeMenuWidgetController::BroadcastInitValues()
{
	Super::BroadcastInitValues();

	UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo);

	for (auto Pair : AS->TagsToAttribute)
	{
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(Pair.Key);
		Info.AttributeValue = Pair.Value().GetNumericValue(AS);
		OnAttributeInfoDelegate.Broadcast(Info);
	}
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	checkf(AttributeSet, TEXT("AttributeSet is not set"));
	UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet);

	for (auto& Pair : AS->TagsToAttribute)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(Pair.Key);
				Info.AttributeValue = Data.NewValue;
				OnAttributeInfoDelegate.Broadcast(Info);
			}
		);
	}
}
