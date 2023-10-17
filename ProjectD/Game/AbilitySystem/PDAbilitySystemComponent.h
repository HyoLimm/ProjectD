// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PDAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTD_API UPDAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:

	/** Sets the current tag relationship mapping, if null it will clear it out */
	//void SetTagRelationshipMapping(UPDAbilityTagRelationshipMapping* NewMapping);


	void ClearAbilityInput();
};
