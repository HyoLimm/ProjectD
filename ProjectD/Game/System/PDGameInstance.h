// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"
#include "PDGameInstance.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class PROJECTD_API UPDGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:

	UPDGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
protected:

	virtual void Init() override;
	virtual void Shutdown() override;
};
