#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindInputActionByTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	for (const FAuraInputAction& Action : AbilityInputActions)
	{
		if (Action.InputTag == Tag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("No input action found for tag %s"), *Tag.ToString());
	}

	return nullptr;
}
