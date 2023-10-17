// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonLocalPlayer.h"
#include "PDLocalPlayer.generated.h"

class UPDSettingsLocal;
class UPDSettingsShared;


UCLASS()
class PROJECTD_API UPDLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()
public:
	UPDLocalPlayer();

	//~UObject interface
	virtual void PostInitProperties() override;
	//~End of UObject interface

	//~UPlayer interface
	virtual void SwitchController(class APlayerController* PC) override;
	//~End of UPlayer interface

	//~ULocalPlayer interface
	virtual bool SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld) override;
	//~End of ULocalPlayer interface



public:
	UFUNCTION()
	UPDSettingsLocal* GetLocalSettings() const;

	UFUNCTION()
	UPDSettingsShared* GetSharedSettings() const;
private:
	void OnPlayerControllerChanged(APlayerController* NewController);


private:
	UPROPERTY(Transient)
	mutable TObjectPtr<UPDSettingsShared> SharedSettings;
};
