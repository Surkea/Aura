#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()


public:

	template<class UserClass, typename PressedFuncType, typename  ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const class UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const class UAuraInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (FAuraInputAction IA: InputConfig->AbilityInputActions)
	{
		if (IA.InputAction && IA.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(IA.InputAction, ETriggerEvent::Started, Object, PressedFunc, IA.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(IA.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, IA.InputTag);
			}
			if (HeldFunc)
			{
				BindAction(IA.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, IA.InputTag);
			}
		}
	}
	
}
