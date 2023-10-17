// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "PDAttributeSet.generated.h"

class UPDAbilitySystemComponent;
class AActor;

class UObject;
class UWorld;
struct FGameplayEffectSpec;






#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// Delegate used to broadcast attribute events.
DECLARE_MULTICAST_DELEGATE_FourParams(FPDAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec& /*EffectSpec*/, float /*EffectMagnitude*/);


UCLASS()
class PROJECTD_API UPDAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UPDAttributeSet();

	UWorld* GetWorld() const override;

	UPDAbilitySystemComponent* GetPDAbilitySystemComponent() const;
};
