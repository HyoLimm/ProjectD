// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "PDHealExecution.generated.h"

class UObject;


/**
 * UPDHealExecution
 *
 *	Execution used by gameplay effects to apply healing to the health attributes.
 */
UCLASS()
class UPDHealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UPDHealExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
