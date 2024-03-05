// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/GameFrameworkComponent.h"

#include "GameplayAbilities/Public/GameplayEffectTypes.h"

#include "PDHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPDHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPDHealth_AttributeChanged, UPDHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);



UENUM(BlueprintType)
enum class EPDDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};

class UPDHealthSet;
class UPDAbilitySystemComponent;

UCLASS()
class PROJECTD_API UPDHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:
	UPDHealthComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "PD|Health")
	static UPDHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UPDHealthComponent>() : nullptr); }

	UFUNCTION(BlueprintCallable, Category = "PD|Health")
	void InitializeWithAbilitySystem(UPDAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "PD|Health")
	void UninitializeFromAbilitySystem();

	UFUNCTION(BlueprintCallable, Category = "PD|Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PD|Health")
	float GetMaxHealth() const;

	// Returns the current health in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "PD|Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "PD|Health")
	EPDDeathState GetDeathState() const { return _DeathState; }


	bool		  IsAlive() const { return _DeathState == EPDDeathState::NotDead; }

	virtual void StartDeath();

	virtual void FinishDeath();

	// Applies enough damage to kill the owner.
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);


public:
	UPROPERTY(BlueprintAssignable)
	FPDHealth_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FPDHealth_AttributeChanged OnMaxHealthChanged;


	UPROPERTY(BlueprintAssignable)
	FPDHealth_DeathEvent OnDeathStarted;

	UPROPERTY(BlueprintAssignable)
	FPDHealth_DeathEvent OnDeathFinished;
protected:
	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

protected:	
	
	
	UPROPERTY()
	TObjectPtr<UPDAbilitySystemComponent> _AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const UPDHealthSet> _HealthSet;
	
	UPROPERTY()
	EPDDeathState _DeathState;
};
