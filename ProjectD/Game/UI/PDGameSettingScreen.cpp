// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/UI/PDGameSettingScreen.h"

#include "Input/CommonUIInputTypes.h"
#include "Game/Player/PDLocalPlayer.h"
#include "Game/Settings/PDGameSettingRegistry.h"

void UPDGameSettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();


	BackHandle = RegisterUIActionBinding(FBindUIActionArgs(BackInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
	ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));
	CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));
}

UGameSettingRegistry* UPDGameSettingScreen::CreateRegistry()
{
	UPDGameSettingRegistry* NewRegistry = NewObject<UPDGameSettingRegistry>();

	if (UPDLocalPlayer* LocalPlayer = CastChecked<UPDLocalPlayer>(GetOwningLocalPlayer()))
	{
		NewRegistry->Initialize(LocalPlayer);
	}

	return NewRegistry;
}

void UPDGameSettingScreen::HandleBackAction()
{
	if (AttemptToPopNavigation())
	{
		return;
	}

	ApplyChanges();

	DeactivateWidget();
}

void UPDGameSettingScreen::HandleApplyAction()
{
	ApplyChanges();
}

void UPDGameSettingScreen::HandleCancelChangesAction()
{
	CancelChanges();
}

void UPDGameSettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
{
	if (bSettingsDirty)
	{
		if (!GetActionBindings().Contains(ApplyHandle))
		{
			AddActionBinding(ApplyHandle);
		}
		if (!GetActionBindings().Contains(CancelChangesHandle))
		{
			AddActionBinding(CancelChangesHandle);
		}
	}
	else
	{
		RemoveActionBinding(ApplyHandle);
		RemoveActionBinding(CancelChangesHandle);
	}
}


