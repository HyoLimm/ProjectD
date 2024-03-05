// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/Foundation/PDButtonBase.h"
#include "CommonActionWidget.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(PDButtonBase)

void UPDButtonBase::SetButtonText(const FText& InText)
{
	bOverride_ButtonText = InText.IsEmpty();
	ButtonText = InText;
	RefreshButtonText();
}

void UPDButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	BP_UpdateButtonStyle();
	RefreshButtonText();
}

void UPDButtonBase::UpdateInputActionWidget()
{
	Super::UpdateInputActionWidget();

	BP_UpdateButtonStyle();
	RefreshButtonText();
}


void UPDButtonBase::RefreshButtonText()
{
	if (bOverride_ButtonText || ButtonText.IsEmpty())
	{
		if (InputActionWidget)
		{
			const FText ActionDisplayText = InputActionWidget->GetDisplayText();
			if (!ActionDisplayText.IsEmpty())
			{
				BP_UpdateButtonText(ActionDisplayText);
				return;
			}
		}
	}

	BP_UpdateButtonText(ButtonText);
}

void UPDButtonBase::OnInputMethodChanged(ECommonInputType CurrentInputType)
{
	Super::OnInputMethodChanged(CurrentInputType);

	BP_UpdateButtonStyle();
}
