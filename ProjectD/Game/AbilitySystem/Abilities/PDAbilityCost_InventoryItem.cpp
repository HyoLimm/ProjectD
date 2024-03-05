// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDAbilityCost_InventoryItem.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDAbilityCost_InventoryItem)

UPDAbilityCost_InventoryItem::UPDAbilityCost_InventoryItem()
{
	Quantity.SetValue(1.0f);
}

bool UPDAbilityCost_InventoryItem::CheckCost(const UPDGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
#if 0
	if (AController* PC = Ability->GetControllerFromActorInfo())
	{
		if (UPDInventoryManagerComponent* InventoryComponent = PC->GetComponentByClass<UPDInventoryManagerComponent>())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);

			return InventoryComponent->GetTotalItemCountByDefinition(ItemDefinition) >= NumItemsToConsume;
		}
	}
#endif
	return false;
}

void UPDAbilityCost_InventoryItem::ApplyCost(const UPDGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
#if 0
	if (ActorInfo->IsNetAuthority())
	{
		if (AController* PC = Ability->GetControllerFromActorInfo())
		{
			if (UPDInventoryManagerComponent* InventoryComponent = PC->GetComponentByClass<UPDInventoryManagerComponent>())
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);

				InventoryComponent->ConsumeItemsByDefinition(ItemDefinition, NumItemsToConsume);
			}
		}
	}
#endif
}

