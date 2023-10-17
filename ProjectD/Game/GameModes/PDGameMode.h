// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModularGameMode.h"

#include "PDGameMode.generated.h"

class UPDPawnData;
class UPDExperienceDefinition;


UCLASS(Config = Game, Meta = (ShortTooltip = "The base game mode class used by this project."))
class PROJECTD_API APDGameMode : public AModularGameModeBase
{
	GENERATED_BODY()
public:

	APDGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "PD|Pawn")
	const UPDPawnData* GetPawnDataForController(const AController* InController) const;

	//~AGameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual void InitGameState() override;
	virtual bool UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	
	
	virtual void GenericPlayerInitialization(AController* NewPlayer) override;
	virtual void FailedToRestartPlayer(AController* NewPlayer) override;
	//~End of AGameModeBase interface

	// 플레이어 봇과 플레이어 모두에 사용할 수 있는 Player Can Restart의 독립적인 버전
	virtual bool ControllerCanRestart(AController* Controller);
protected:
	bool IsExperienceLoaded() const;
	void HandleMatchAssignmentIfNotExpectingOne();

	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);


	void OnExperienceLoaded(const UPDExperienceDefinition* CurrentExperience);
};
