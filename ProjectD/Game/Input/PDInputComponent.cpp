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
	check(InputConfig);
	check(InputSubsystem);

	UPDLocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<UPDLocalPlayer>();
	check(LocalPlayer);

	// �������� ��ϵ� �Է� ������ �߰��մϴ�!
	if (UPDSettingsLocal* LocalSettings = UPDSettingsLocal::Get())
	{
		// Tell enhanced input about any custom keymappings that the player may have customized
		// �÷��̾ ����� ������ ����� ���� Ű ���ο� ���� enhanced input ���� ����
		for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		{
			if (Pair.Key != NAME_None && Pair.Value.IsValid())
			{
				InputSubsystem->AddPlayerMappedKeyInSlot(Pair.Key, Pair.Value);
			}
		}
	}
}

void UPDInputComponent::RemoveInputMappings(const UPDInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	APDPlayerCharacter* LocalPlayer = InputSubsystem->GetLocalPlayer<APDPlayerCharacter>();
	check(LocalPlayer);

	//if (UPDSettingsLocal* LocalSettings = UPDSettingsLocal::Get())
	//{
	//	// Remove any registered input contexts
	//	const TArray<FLoadedMappableConfigPair>& Configs = LocalSettings->GetAllRegisteredInputConfigs();
	//	for (const FLoadedMappableConfigPair& Pair : Configs)
	//	{
	//		InputSubsystem->RemovePlayerMappableConfig(Pair.Config);
	//	}

	//	// Clear any player mapped keys from enhanced input
	//	for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
	//	{
	//		InputSubsystem->RemovePlayerMappedKeyInSlot(Pair.Key);
	//	}
	//}
}

void UPDInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
