// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PDCombatBaseComponent.generated.h"


UCLASS( Abstract )
class PROJECTD_API UPDCombatBaseComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UPDCombatBaseComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
