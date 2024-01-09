// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "PDPlayerController.generated.h"

/**
 * 
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class PROJECTD_API APDPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
public:
	APDPlayerController(const FObjectInitializer& ObjectInitializer);


	//~APlayerController interface
	virtual void ReceivedPlayer() override;
};
