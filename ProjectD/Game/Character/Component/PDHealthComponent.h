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


struct FHealthPoint;


UCLASS()
class PROJECTD_API UPDHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:
	UPDHealthComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the health component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "PD|Health")
	static UPDHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UPDHealthComponent>() : nullptr); }


	UPROPERTY(BlueprintAssignable)
	FPDHealth_DeathEvent _OnDeathStarted;

	UPROPERTY(BlueprintAssignable)
	FPDHealth_DeathEvent _OnDeathFinished;


	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "PD|Health")
	void InitializeWithAbilitySystem();

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
	// Delegate fired when the health value has changed.
	UPROPERTY(BlueprintAssignable)
	FPDHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed.
	UPROPERTY(BlueprintAssignable)
	FPDHealth_AttributeChanged OnMaxHealthChanged;
protected:
	virtual void OnUnregister() override;

	void ClearGameplayTags();

	/*virtual void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);*/

protected:	
	TUniquePtr<FHealthPoint> _HealthData;
	
	UPROPERTY()
	EPDDeathState _DeathState;
};
