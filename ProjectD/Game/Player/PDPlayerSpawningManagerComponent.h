// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "PDPlayerSpawningManagerComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTD_API UPDPlayerSpawningManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UPDPlayerSpawningManagerComponent(const FObjectInitializer& ObjectInitializer);
private:
	bool ControllerCanRestart(AController* Player);
	friend class APDGameMode;
};
