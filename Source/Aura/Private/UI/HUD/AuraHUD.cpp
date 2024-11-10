#include "UI/HUD/AuraHUD.h"

#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/Widgets/AuraUserWidget.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& InParams)
{
	if(OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetParams(InParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}

	return  OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{

	checkf(OverlayWidgetClass, TEXT("Please set OverlayWidgetClass a valid class in BP_AuraHUD."));
	checkf(OverlayWidgetControllerClass, TEXT("Please set OverlayWidgetControllerClass a valid class in BP_AuraHUD."));

	const FWidgetControllerParams InitParams (PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(InitParams);
	
	UAuraUserWidget* Widget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	Widget->SetWidgetController(WidgetController);

	WidgetController->BroadcastInitValues();
	Widget->AddToViewport();
	

	
}

	