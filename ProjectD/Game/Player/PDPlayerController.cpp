// Fill out your copyright notice in the Description page of Project Settings.


#include "PDPlayerController.h"
#include "Game/Player/PDCheatManager.h"

APDPlayerController::APDPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CheatClass = UPDCheatManager::StaticClass();
}

void APDPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}
