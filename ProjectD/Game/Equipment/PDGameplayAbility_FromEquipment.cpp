// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDGameplayAbility_FromEquipment.h"
#include "PDEquipmentInstance.h"
#include "Game/Inventory/PDInventoryItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDGameplayAbility_FromEquipment)

UPDGameplayAbility_FromEquipment::UPDGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UPDEquipmentInstance* UPDGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UPDEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

UPDInventoryItemInstance* UPDGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (UPDEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<UPDInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult UPDGameplayAbility_FromEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		Context.AddError(NSLOCTEXT("PD", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif
