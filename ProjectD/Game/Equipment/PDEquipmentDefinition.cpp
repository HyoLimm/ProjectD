// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDEquipmentDefinition.h"
#include "PDEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDEquipmentDefinition)

UPDEquipmentDefinition::UPDEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = UPDEquipmentInstance::StaticClass();
}

