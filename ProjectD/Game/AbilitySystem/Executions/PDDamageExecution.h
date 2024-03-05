// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "PDDamageExecution.generated.h"

class UObject;


/**
 * UPDDamageExecution
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class UPDDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UPDDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
