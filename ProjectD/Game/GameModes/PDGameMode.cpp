// Fill out your copyright notice in the Description page of Project Settings.


#include "PDGameMode.h"
#include "Game/Character/PDPawnData.h"
#include "Game/Player/PDPlayerState.h"
#include "Game/Player/PDPlayerSpawningManagerComponent.h"
#include "Game/Player/PDPlayerController.h"
#include "Game/PDLogChannels.h"
#include "Game/Character/Component/PDPawnExtensionComponent.h"
#include "Game/Character/PDBaseCharacter.h"
#include "Game/System/PDAssetManager.h"
#include "Game/UI/PDHUD.h"
#include "PDExperienceManagerComponent.h"
#include "PDExperienceDefinition.h"
#include "PDGameState.h"
#include "Game/GameModes/PDEnemyManagerComponent.h"
#include "Game/Development/PDDeveloperSettings.h"
#include "Game/GameModes/PDWorldSettings.h"

APDGameMode::APDGameMode(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	GameStateClass = APDGameState::StaticClass();	
	PlayerControllerClass = APDPlayerController::StaticClass();	
	PlayerStateClass = APDPlayerState::StaticClass();
	DefaultPawnClass = APDBaseCharacter::StaticClass();
	HUDClass = APDHUD::StaticClass();

	EnemyManagerComponent = CreateDefaultSubobject<UPDEnemyManagerComponent>(TEXT("EnemyManagerComponent"));
}



void APDGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// ���� �����ӿ��� ���� ������ �ʱ�ȭ�� �ð��� �־��� ������ ��ٸ��ϴ�
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void APDGameMode::InitGameState()
{
	Super::InitGameState();

	// Listen for the experience load to complete	
	UPDExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UPDExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnPDExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

bool APDGameMode::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	// �ƹ��͵� ���� ������. �÷��̾ ��¥�� ����Ϸ��� �� �� PostLogin���� ��ٸ��ڽ��ϴ�.
	// ���� ���� �ƹ��͵� �ϴ� ���� ���� �ʽ��ϴ�. �� ������ ���� �ý����� ���� ��������� �ʾҽ��ϴ�.
	return true;
}

bool APDGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	// We never want to use the start spot, always use the .
	//�츮�� ���� ���� ������ ����ϰ� ���� �ʽ��ϴ�. �׻� spawn management component�� ����մϴ�.
	return false;
}

void APDGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	//if (UPDPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UPDPlayerSpawningManagerComponent>())
	//{
	//	PlayerSpawningComponent->FinishRestartPlayer(NewPlayer, StartRotation);
	//}

	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

void APDGameMode::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);
}

void APDGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);
}

AActor* APDGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	return Super::ChoosePlayerStart_Implementation(Player);
}

void APDGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Delay starting new players until the experience has been loaded
	// (players who log in prior to that will be started by OnExperienceLoaded)
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

bool APDGameMode::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		// Bot version of Super::PlayerCanRestart_Implementation
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	//if (UPDPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<UPDPlayerSpawningManagerComponent>())
	//{
	//	return PlayerSpawningComponent->ControllerCanRestart(Controller);
	//}

	return true;
}

bool APDGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

UClass* APDGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UPDPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
} 

bool APDGameMode::IsExperienceLoaded() const
{
	check(GameState);
	UPDExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UPDExperienceManagerComponent>();
	check(ExperienceComponent);

	return ExperienceComponent->IsExperienceLoaded();
}

void APDGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	UWorld* World = GetWorld();
	if (!ExperienceId.IsValid() && World->IsPlayInEditor())
	{
		ExperienceId = GetDefault<UPDDeveloperSettings>()->ExperienceOverride;
		ExperienceIdSource = TEXT("DeveloperSettings");
	}

	// see if the world settings has a default experience
	if (!ExperienceId.IsValid())
	{
		if (APDWorldSettings* TypedWorldSettings = Cast<APDWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	FAssetData Dummy;
	if (ExperienceId.IsValid() && false == UPDAssetManager::Get().GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		UE_LOG(LogPD, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();
	}



	// Final fallback to the default experience
	if (!ExperienceId.IsValid())
	{
		//@TODO: Pull this from a config setting or something
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("PDExperienceDefinition"), FName("BP_PDDefaultExperience"));
		ExperienceIdSource = TEXT("Default");
	}

	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

void APDGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogPD, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);

		UPDExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UPDExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->SetCurrentExperience(ExperienceId);
	}
	else
	{
		UE_LOG(LogPD, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
}

void APDGameMode::OnExperienceLoaded(const UPDExperienceDefinition* CurrentExperience)
{
	// Spawn any players that are already attached
	//@TODO: Here we're handling only *player* controllers, but in GetDefaultPawnClassForController_Implementation we skipped all controllers
	// GetDefaultPawnClassForController_Implementation might only be getting called for players anyways
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

const UPDPawnData* APDGameMode::GetPawnDataForController(const AController* InController) const
{	
	// Player State�� �� �����Ͱ� �̹� �����Ǿ� �ִ��� Ȯ���մϴ�
	if (InController != nullptr)
	{
		if (const APDPlayerState* PlayerState = InController->GetPlayerState<APDPlayerState>())
		{
			if (const UPDPawnData* PawnData = PlayerState->GetPawnData<UPDPawnData>())
			{
				return PawnData;
			}
		}
	}
	
	// �׷��� ���� ���, ���� ȯ���� �⺻������ ���ư��ϴ�
	check(GameState);
	UPDExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UPDExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded())
	{
		const UPDExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;
		}

		// ����ġ(Experience)�� �ε�Ǿ����� ������ �� �����Ͱ� �����ϴ�. �ϴ� �⺻������ ���ư��ϴ�
		return UPDAssetManager::Get().GetDefaultPawnData();
	}

	//����ġ(Experience)�� ���� �ε���� �ʾ����Ƿ� ������ �� �����Ͱ� �����ϴ�
	return nullptr;
}

APawn* APDGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// �⺻ �÷��̾� ���� �ʿ� �������� ���ʽÿ�.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UPDPawnExtensionComponent* PawnExtComp = UPDPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UPDPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogPD, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogPD, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogPD, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

