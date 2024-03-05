// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDBotCreationComponent.h"
#include "PDGameMode.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "Game/GameModes/PDExperienceManagerComponent.h"
#include "Game/Development/PDDeveloperSettings.h"
#include "Game/Character/Component/PDPawnExtensionComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Game/Character/Component/PDHealthComponent.h"
#include "Game/PDLogChannels.h"
#include "../Character/Enemy/PDEnemyBaseCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDBotCreationComponent)

UPDBotCreationComponent::UPDBotCreationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPDBotCreationComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for the experience load to complete
	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	UPDExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UPDExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded_LowPriority(FOnPDExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void UPDBotCreationComponent::OnExperienceLoaded(const UPDExperienceDefinition* Experience)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		ServerCreateBots();
	}
#endif
}

#if WITH_SERVER_CODE

void UPDBotCreationComponent::ServerCreateBots()
{
	if (BotControllerClass == nullptr)
	{
		return;
	}

	RemainingBotNames = RandomBotNames;

	// Determine how many bots to spawn
	int32 EffectiveBotCount = NumBotsToCreate;

	// Give the developer settings a chance to override it
	if (GIsEditor)
	{
		const UPDDeveloperSettings* DeveloperSettings = GetDefault<UPDDeveloperSettings>();
		
		if (DeveloperSettings->bOverrideBotCount)
		{
			EffectiveBotCount = DeveloperSettings->OverrideNumPlayerBotsToSpawn;
		}
	}

	// Give the URL a chance to override it
	if (AGameModeBase* GameModeBase = GetGameMode<AGameModeBase>())
	{
		EffectiveBotCount = UGameplayStatics::GetIntOption(GameModeBase->OptionsString, TEXT("NumBots"), EffectiveBotCount);
	}

	// Create them
	for (int32 Count = 0; Count < EffectiveBotCount; ++Count)
	{
		SpawnOneBot();
	}
}

FString UPDBotCreationComponent::CreateBotName(int32 PlayerIndex)
{
	FString Result;
	if (RemainingBotNames.Num() > 0)
	{
		const int32 NameIndex = FMath::RandRange(0, RemainingBotNames.Num() - 1);
		Result = RemainingBotNames[NameIndex];
		RemainingBotNames.RemoveAtSwap(NameIndex);
	}
	else
	{
		//@TODO: PlayerId is only being initialized for players right now
		PlayerIndex = FMath::RandRange(260, 260+100);
		Result = FString::Printf(TEXT("Tinplate %d"), PlayerIndex);
	}
	return Result;
}

APDEnemyBaseCharacter* UPDBotCreationComponent::SpawnDefaultPawnFor(AController* NewPlayer, AActor* StartSpot)
{
	// Don't allow pawn to be spawned with any pitch or roll
	FRotator StartRotation(ForceInit);
	StartRotation.Yaw = StartSpot->GetActorRotation().Yaw;
	FVector StartLocation = StartSpot->GetActorLocation();

	FTransform Transform = FTransform(StartRotation, StartLocation);
	return SpawnDefaultPawnAtTransform(NewPlayer, Transform);
}

APDEnemyBaseCharacter* UPDBotCreationComponent::SpawnDefaultPawnAtTransform(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	//SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save default player pawns into a map

	APDEnemyBaseCharacter* ResultPawn = GetWorld()->SpawnActor<APDEnemyBaseCharacter>(EnemyCharacterClass, SpawnTransform, SpawnInfo);
	if (!ResultPawn)
	{
		//UE_LOG(LogPD, Warning, TEXT("SpawnDefaultPawnAtTransform: Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(APDEnemyBaseCharacter), *SpawnTransform.ToHumanReadableString());
	}
	return ResultPawn;
}

void UPDBotCreationComponent::SpawnOneBot()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.OverrideLevel = GetComponentLevel();
	SpawnInfo.ObjectFlags |= RF_Transient;
	
	if (AAIController* NewController = GetWorld()->SpawnActor<AAIController>(BotControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo))
	{
		APDGameMode* GameMode = GetGameMode<APDGameMode>();
		check(GameMode);

		if (NewController->PlayerState != nullptr)
		{
			NewController->PlayerState->SetPlayerName(CreateBotName(NewController->PlayerState->GetPlayerId()));
		}

		GameMode->GenericPlayerInitialization(NewController);

		AActor* StartSpot = GameMode->FindPlayerStart(NewController);		
		if (APawn* NewPawn = SpawnDefaultPawnFor(NewController, StartSpot))
		{
			NewController->SetPawn(NewPawn);
			NewController->Possess(NewPawn);
		}
		

		if (NewController->GetPawn() != nullptr)
		{
			if (UPDPawnExtensionComponent* PawnExtComponent = NewController->GetPawn()->FindComponentByClass<UPDPawnExtensionComponent>())
			{
				PawnExtComponent->CheckDefaultInitialization();
			}
		}

		SpawnedBotList.Add(NewController);
	}
}

void UPDBotCreationComponent::RemoveOneBot()
{
	if (SpawnedBotList.Num() > 0)
	{
		// Right now this removes a random bot as they're all the same; could prefer to remove one
		// that's high skill or low skill or etc... depending on why you are removing one
		const int32 BotToRemoveIndex = FMath::RandRange(0, SpawnedBotList.Num() - 1);

		AAIController* BotToRemove = SpawnedBotList[BotToRemoveIndex];
		SpawnedBotList.RemoveAtSwap(BotToRemoveIndex);

		if (BotToRemove)
		{
			// If we can find a health component, self-destruct it, otherwise just destroy the actor
			if (APawn* ControlledPawn = BotToRemove->GetPawn())
			{
				if (UPDHealthComponent* HealthComponent = UPDHealthComponent::FindHealthComponent(ControlledPawn))
				{
					// Note, right now this doesn't work quite as desired: as soon as the player state goes away when
					// the controller is destroyed, the abilities like the death animation will be interrupted immediately
					HealthComponent->DamageSelfDestruct();
				}
				else
				{
					ControlledPawn->Destroy();
				}
			}

			// Destroy the controller (will cause it to Logout, etc...)
			BotToRemove->Destroy();
		}
	}
}

#endif
