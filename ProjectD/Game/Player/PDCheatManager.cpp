// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Player/PDCheatManager.h"

#include "Kismet/KismetInternationalizationLibrary.h"

#include "Game/Player/PDPlayerController.h"
#include "Game/Character/Component/PDPlayerLevelManagerComponent.h"

void UPDCheatManager::AddPlayerExpr(float value)
{
	if (APDPlayerController* PlayerController = Cast<APDPlayerController>(GetOuterAPlayerController()))
	{
		if (const auto pawn = UPDPlayerLevelManagerComponent::FindPlayerLevelMangerComponent(PlayerController->GetPawn()))
		{
			pawn->AddExperience(value);
		}
	}
}

void UPDCheatManager::ChangeLanguage(const FString& language)
{
	UKismetInternationalizationLibrary::SetCurrentCulture(language);
}
