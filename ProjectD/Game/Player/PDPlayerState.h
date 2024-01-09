// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerState.h"
#include "PDPlayerState.generated.h"

class UPDExperienceDefinition;
class UPDPawnData;


UCLASS(Config = Game)
class PROJECTD_API APDPlayerState : public AModularPlayerState
{
	GENERATED_BODY()
public:

	APDPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());






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


};
