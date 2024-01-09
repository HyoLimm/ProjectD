// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameModes/PDGameState.h"
#include "Game/GameModes/PDExperienceManagerComponent.h"


APDGameState::APDGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;



	ExperienceManagerComponent = CreateDefaultSubobject<UPDExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}

void APDGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void APDGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APDGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APDGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void APDGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
}
