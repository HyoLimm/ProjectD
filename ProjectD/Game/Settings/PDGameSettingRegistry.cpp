// Copyright(c) Depsee. All Rights Reserved.


#include "Game/Settings/PDGameSettingRegistry.h"
#include "GameSettingCollection.h"

#include "Game/Player/PDLocalPlayer.h"
UPDGameSettingRegistry::UPDGameSettingRegistry()
{

}

UPDGameSettingRegistry* UPDGameSettingRegistry::Get(UPDLocalPlayer* InLocalPlayer)
{
	UPDGameSettingRegistry* Registry = FindObject<UPDGameSettingRegistry>(InLocalPlayer, TEXT("PDGameSettingRegistry"), true);
	if (Registry == nullptr)
	{
		Registry = NewObject<UPDGameSettingRegistry>(InLocalPlayer, TEXT("PDGameSettingRegistry"));
		Registry->Initialize(InLocalPlayer);
	}

	return Registry;
}

void UPDGameSettingRegistry::SaveChanges()
{

}

void UPDGameSettingRegistry::OnInitialize(ULocalPlayer* InLocalPlayer)
{
	UPDLocalPlayer* PDLocalPlayer = Cast<UPDLocalPlayer>(InLocalPlayer);

	GameplaySettings = InitializeGameplaySettings(PDLocalPlayer);
	RegisterSetting(GameplaySettings);
}

bool UPDGameSettingRegistry::IsFinishedInitializing() const
{
	if (Super::IsFinishedInitializing())
	{
		if (UPDLocalPlayer* LocalPlayer = Cast<UPDLocalPlayer>(OwningLocalPlayer))
		{
			if (LocalPlayer->GetSharedSettings() == nullptr)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

