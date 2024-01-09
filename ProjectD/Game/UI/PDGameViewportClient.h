// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameViewportClient.h"
#include "PDGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTD_API UPDGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()
public:
	UPDGameViewportClient();

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;
};
