// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameStateComponent.h"

#include "PDBotCreationComponent.generated.h"

class UPDExperienceDefinition;
class UPDPawnData;
class AAIController;
class APDEnemyBaseCharacter;

UCLASS(Blueprintable, Abstract)
class UPDBotCreationComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UPDBotCreationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	//~End of UActorComponent interface

private:
	void OnExperienceLoaded(const UPDExperienceDefinition* Experience);

protected:
	UPROPERTY(EditDefaultsOnly, Category=Teams)
	int32 NumBotsToCreate = 5;

	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TSubclassOf<AAIController> BotControllerClass;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TSubclassOf<APDEnemyBaseCharacter> EnemyCharacterClass;

	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TArray<FString> RandomBotNames;

	TArray<FString> RemainingBotNames;

protected:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AAIController>> SpawnedBotList;

#if WITH_SERVER_CODE
public:
	void Cheat_AddBot() { SpawnOneBot(); }
	void Cheat_RemoveBot() { RemoveOneBot(); }

protected:
	virtual void ServerCreateBots();

	virtual void SpawnOneBot();
	virtual void RemoveOneBot();

	FString CreateBotName(int32 PlayerIndex);
#endif

protected:

	APDEnemyBaseCharacter* SpawnDefaultPawnFor(AController* NewPlayer, AActor* StartSpot);
	APDEnemyBaseCharacter* SpawnDefaultPawnAtTransform(AController* NewPlayer, const FTransform& SpawnTransform);
};
