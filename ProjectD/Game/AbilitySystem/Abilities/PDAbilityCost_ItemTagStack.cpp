// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDAbilityCost_ItemTagStack.h"

#include "Game/Equipment/PDGameplayAbility_FromEquipment.h"
#include "Game/Inventory/PDInventoryItemInstance.h"
#include "NativeGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDAbilityCost_ItemTagStack)

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_FAIL_COST, "Ability.ActivateFail.Cost");

UPDAbilityCost_ItemTagStack::UPDAbilityCost_ItemTagStack()
{
	Quantity.SetValue(1.0f);
	FailureTag = TAG_ABILITY_FAIL_COST;
}

bool UPDAbilityCost_ItemTagStack::CheckCost(const UPDGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (const UPDGameplayAbility_FromEquipment* EquipmentAbility = Cast<const UPDGameplayAbility_FromEquipment>(Ability))
	{
		if (UPDInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);
			const bool bCanApplyCost = ItemInstance->GetStatTagStackCount(Tag) >= NumStacks;

			// Inform other abilities why this cost cannot be applied
			if (!bCanApplyCost && OptionalRelevantTags && FailureTag.IsValid())
			{
				OptionalRelevantTags->AddTag(FailureTag);				
			}
			return bCanApplyCost;
		}
	}
	return false;
}

void UPDAbilityCost_ItemTagStack::ApplyCost(const UPDGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (const UPDGameplayAbility_FromEquipment* EquipmentAbility = Cast<const UPDGameplayAbility_FromEquipment>(Ability))
		{
			if (UPDInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

				ItemInstance->RemoveStatTagStack(Tag, NumStacks);
			}
		}
	}
}

