// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/GameFrameworkComponent.h"
#include "GameplayAbilities/Public/GameplayEffectTypes.h"
#include "PDHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPDHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPDHealth_AttributeChanged, UPDHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

class UPDAbilitySystemComponent;
class UPDHealthSet;



UCLASS()
class PROJECTD_API UPDHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:
	UPDHealthComponent(const FObjectInitializer& ObjectInitializer);


public:
	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FPDHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FPDHealth_DeathEvent OnDeathFinished;


	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "PD|Health")
	void InitializeWithAbilitySystem(UPDAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "PD|Health")
	void UninitializeFromAbilitySystem();
public:
	// Applies enough damage to kill the owner.
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);


public:
	// Delegate fired when the health value has changed.
	UPROPERTY(BlueprintAssignable)
	FPDHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed.
	UPROPERTY(BlueprintAssignable)
	FPDHealth_AttributeChanged OnMaxHealthChanged;


protected:
	virtual void OnUnregister() override;

	void ClearGameplayTags();


	virtual void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

protected:

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const UPDHealthSet> HealthSet;



	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<UPDAbilitySystemComponent> AbilitySystemComponent;
};
