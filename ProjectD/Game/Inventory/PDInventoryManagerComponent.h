// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "PDInventoryManagerComponent.generated.h"

class UPDInventoryItemDefinition;
class UPDInventoryItemInstance;
class UPDInventoryManagerComponent;
class UObject;
struct FFrame;
struct FPDInventoryList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FPDInventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UPDInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};

/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct FPDInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FPDInventoryEntry()
	{}

	FString GetDebugString() const;

private:
	friend FPDInventoryList;
	friend UPDInventoryManagerComponent;

	UPROPERTY()
	TObjectPtr<UPDInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FPDInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FPDInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FPDInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	TArray<UPDInventoryItemInstance*> GetAllItems() const;

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FPDInventoryEntry, FPDInventoryList>(Entries, DeltaParms, *this);
	}

	UPDInventoryItemInstance* AddEntry(TSubclassOf<UPDInventoryItemDefinition> ItemClass, int32 StackCount);
	void AddEntry(UPDInventoryItemInstance* Instance);

	void RemoveEntry(UPDInventoryItemInstance* Instance);

private:
	void BroadcastChangeMessage(FPDInventoryEntry& Entry, int32 OldCount, int32 NewCount);

private:
	friend UPDInventoryManagerComponent;

private:
	// Replicated list of items
	UPROPERTY()
	TArray<FPDInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FPDInventoryList> : public TStructOpsTypeTraitsBase2<FPDInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages an inventory
 */
UCLASS(BlueprintType)
class PROJECTD_API UPDInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPDInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	UFUNCTION(BlueprintPure, Category = "PD|InventoryManager")
	static UPDInventoryManagerComponent* FindInventoryManagerComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UPDInventoryManagerComponent>() : nullptr);}
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	bool CanAddItemDefinition(TSubclassOf<UPDInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	UPDInventoryItemInstance* AddItemDefinition(TSubclassOf<UPDInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddItemInstance(UPDInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void RemoveItemInstance(UPDInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<UPDInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	UPDInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UPDInventoryItemDefinition> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<UPDInventoryItemDefinition> ItemDef) const;
	bool ConsumeItemsByDefinition(TSubclassOf<UPDInventoryItemDefinition> ItemDef, int32 NumToConsume);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

private:
	UPROPERTY(Replicated)
	FPDInventoryList InventoryList;
};
