// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerState.h"
#include "AbilitySystemInterface.h"
#include "PDPlayerState.generated.h"

class UPDExperienceDefinition;
class UPDPawnData;
class UPDAbilitySystemComponent;
class UAbilitySystemComponent;


UCLASS(Config = Game)
class PROJECTD_API APDPlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:

	APDPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	UFUNCTION(BlueprintCallable, Category = "PD|PlayerState")
	UPDAbilitySystemComponent* GetPDAbilitySystemComponent() const { return AbilitySystemComponent; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UPDPawnData* InPawnData);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	static const FName NAME_PDAbilityReady;
private:

	void OnExperienceLoaded(const UPDExperienceDefinition* CurrentExperience);
protected:

	UPROPERTY()
	TObjectPtr<const UPDPawnData> PawnData;
private:
	UPROPERTY(VisibleAnywhere, Category = "PD|PlayerState")
	TObjectPtr<UPDAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<const class UPDHealthSet> HealthSet;
	
	UPROPERTY()
	TObjectPtr<const class UPDCombatSet> CombatSet;

	UPROPERTY()
	TObjectPtr<const class UPDExperienceSet> ExpSet;
};
