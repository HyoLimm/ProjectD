// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Character/PDBaseCharacter.h"
#include "AbilitySystemInterface.h"
#include "PDEnemyBaseCharacter.generated.h"

class UPDAIBehaviorComponent;
class UPDAbilitySystemComponent;
class UGameplayAbility;


UCLASS()
class PROJECTD_API APDEnemyBaseCharacter : public APDBaseCharacter
{
	GENERATED_BODY()
	
public:
	APDEnemyBaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PD| EnemyCharacter", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDAIBehaviorComponent> BehaviorComponent;	

	//UFUNCTION(BlueprintCallable, Category = "PD|PlayerState")
	UPDAbilitySystemComponent* GetPDAbilitySystemComponent() const { return AbilitySystemComponent; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void AddCharacterAbilites();

	virtual void OnDeathStarted(AActor* OwningActor) override;
protected:
	virtual void OnAbilitySystemInitialized() override;
	virtual void OnAbilitySystemUninitialized() override;
private:
	UPROPERTY(VisibleAnywhere, Category = "PD|PlayerState")
	TObjectPtr<UPDAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const class UPDHealthSet> HealthSet;

	UPROPERTY()
	TObjectPtr<const class UPDCombatSet> CombatSet;
};
