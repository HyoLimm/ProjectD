// Fill out your copyright notice in the Description page of Project Settings.


#include "PDInputComponent.h"
#include "EnhancedInputSubsystems.h"


#include "Game/Character/PDPlayerCharacter.h"
#include "Game/Settings/PDSettingsLocal.h"
#include "Game/Player/PDLocalPlayer.h"



UPDInputComponent::UPDInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPDInputComponent::AddInputMappings(const UPDInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{

}

void UPDInputComponent::RemoveInputMappings(const UPDInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{

}

void UPDInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
