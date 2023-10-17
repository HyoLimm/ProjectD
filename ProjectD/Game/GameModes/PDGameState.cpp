// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameModes/PDGameState.h"
#include "Game/GameModes/PDExperienceManagerComponent.h"
#include "Game/AbilitySystem/PDAbilitySystemComponent.h"

APDGameState::APDGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UPDAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	//AbilitySystemComponent->SetIsReplicated(true);
	//AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

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

UAbilitySystemComponent* APDGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
