// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDBaseCharacter.h"
#include "InputActionValue.h"
#include "PDPlayerCharacter.generated.h"

class UPDCameraComponent;
class UPDPlayerLevelManagerComponent;
class UPDPlayerCombatComponent;

UCLASS()
class PROJECTD_API APDPlayerCharacter : public APDBaseCharacter
{
	GENERATED_BODY()
public:
	APDPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PD|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PD|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDPlayerLevelManagerComponent> ExperienceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PD|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDPlayerCombatComponent>		CombatComponent;


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

protected:
	virtual void OnAbilitySystemInitialized() override;
	virtual void OnAbilitySystemUninitialized() override;
};
