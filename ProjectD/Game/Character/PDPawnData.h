// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PDPawnData.generated.h"

class APawn;
class UPDCameraMode;
class UPDInputConfig;
class UPDAbilitySet;


UCLASS(BlueprintType, Const, Meta = (DisplayName = "PD Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class PROJECTD_API UPDPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPDPawnData(const FObjectInitializer& ObjectInitializer);

public:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PD|Pawn")
	TSubclassOf<APawn> PawnClass;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PD|Input")
	TObjectPtr<UPDInputConfig> InputConfig;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PD|Camera")
	TSubclassOf<UPDCameraMode> DefaultCameraMode;



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PD|Abilities")
	TArray<TObjectPtr<UPDAbilitySet>> AbilitySets;
};
