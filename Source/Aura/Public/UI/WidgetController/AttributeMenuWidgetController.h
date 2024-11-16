#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, FAuraAttributeInfo, Attribute);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature OnAttributeInfoDelegate;

	virtual void BroadcastInitValues() override;
	virtual void BindCallbacksToDependencies() override;

protected:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;
	
};
