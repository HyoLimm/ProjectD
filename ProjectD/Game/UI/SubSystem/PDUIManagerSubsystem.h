// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameUIManagerSubsystem.h"
#include "PDUIManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTD_API UPDUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()
public:

	UPDUIManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();

	FTSTicker::FDelegateHandle TickHandle;
};
