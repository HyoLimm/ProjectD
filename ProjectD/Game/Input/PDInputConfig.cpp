// Fill out your copyright notice in the Description page of Project Settings.


#include "PDInputConfig.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDInputConfig)

UPDInputConfig::UPDInputConfig(const FObjectInitializer& ObjectInitializer)
{

}

const UInputAction* UPDInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound /*= true*/) const
{
	for (const FPDInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		//UE_LOG(Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));	}
		}
	return nullptr;
}

const UInputAction* UPDInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound /*= true*/) const
{
	for (const FPDInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		//UE_LOG(Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
