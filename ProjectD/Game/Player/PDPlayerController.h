// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "PDPlayerController.generated.h"

class UPDSettingsShared;

UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class PROJECTD_API APDPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
public:
	APDPlayerController(const FObjectInitializer& ObjectInitializer);


	UFUNCTION(BlueprintCallable, Category = "PD|PlayerController")
	APDPlayerState* GetPDPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "PD|PlayerController")
	UPDAbilitySystemComponent* GetPDAbilitySystemComponent() const;

	//~APlayerController interface
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void AddCheats(bool bForce) override;
	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface


protected:
	void OnSettingsChanged(UPDSettingsShared* Settings);
};
