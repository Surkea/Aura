	


#include "UI/WidgetController/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetParams(const FWidgetControllerParams& InWidgetControllerParams)
{
	PlayerController = InWidgetControllerParams.PlayerController;
	PlayerState = InWidgetControllerParams.PlayerState;
	AbilitySystemComponent = InWidgetControllerParams.AbilitySystemComponent;
	AttributeSet = InWidgetControllerParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitValues()
{
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}
