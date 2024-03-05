// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "PDInventoryItemDefinition.generated.h"

template <typename T> class TSubclassOf;

class UPDInventoryItemInstance;
struct FFrame;

//////////////////////////////////////////////////////////////////////

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class PROJECTD_API UPDInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UPDInventoryItemInstance* Instance) const {}
};

//////////////////////////////////////////////////////////////////////

/**
 * UPDInventoryItemDefinition
 */
UCLASS(Blueprintable, Const, Abstract)
class UPDInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UPDInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UPDInventoryItemFragment>> Fragments;

public:
	const UPDInventoryItemFragment* FindFragmentByClass(TSubclassOf<UPDInventoryItemFragment> FragmentClass) const;
};

//@TODO: Make into a subsystem instead?
UCLASS()
class UPDInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UPDInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UPDInventoryItemDefinition> ItemDef, TSubclassOf<UPDInventoryItemFragment> FragmentClass);
};
