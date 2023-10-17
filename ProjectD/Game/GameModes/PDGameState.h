// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "ModularGameState.h"

#include "PDGameState.generated.h"

class UPDExperienceManagerComponent;
class UPDAbilitySystemComponent;
class UAbilitySystemComponent;

UCLASS(Config = Game)
class PROJECTD_API APDGameState : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:

	APDGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());



	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~AGameStateBase interface
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;	
	//~End of AGameStateBase interface

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

	UFUNCTION(BlueprintCallable, Category = "PD|GameState")
	UPDAbilitySystemComponent* GetPDAbilitySystemComponent() const { return AbilitySystemComponent; }



private:
	UPROPERTY()
	TObjectPtr<UPDExperienceManagerComponent> ExperienceManagerComponent;

	// The ability system component subobject for game-wide things (primarily gameplay cues)
	UPROPERTY(VisibleAnywhere, Category = "PD|GameState")
	TObjectPtr<UPDAbilitySystemComponent> AbilitySystemComponent;

protected:

	//virtual void Tick(float DeltaSeconds) override;
};
