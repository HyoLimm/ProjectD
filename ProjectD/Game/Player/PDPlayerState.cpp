// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Player/PDPlayerState.h"
#include "Game/GameModes/PDExperienceManagerComponent.h"
#include "Game/GameModes/PDGameMode.h"
#include "Game/AbilitySystem/PDAbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Game/AbilitySystem/PDAbilitySet.h"

const FName APDPlayerState::NAME_PDAbilityReady("PDAbilitiesReady");


APDPlayerState::APDPlayerState(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UPDAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
}




void APDPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}


void APDPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();


	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld())
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		UPDExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UPDExperienceManagerComponent>();
		check(ExperienceComponent);

		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnPDExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void APDPlayerState::OnExperienceLoaded(const UPDExperienceDefinition* CurrentExperience)
{
	if (APDGameMode* PDGameMode = GetWorld()->GetAuthGameMode<APDGameMode>())
	{
		if (const UPDPawnData* NewPawnData = PDGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("ALyraPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}


void APDPlayerState::SetPawnData(const UPDPawnData* InPawnData)
{
	check(InPawnData);

	if (PawnData)
	{
		//UE_LOG(LogLyra, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	//MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	//for (const UPDAbilitySet* AbilitySet : InPawnData->AbilitySets)
	//{
	//	if (AbilitySet)
	//	{
	//		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
	//	}
	//}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_PDAbilityReady);
}