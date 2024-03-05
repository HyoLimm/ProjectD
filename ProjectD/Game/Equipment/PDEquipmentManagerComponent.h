// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Game/AbilitySystem/PDAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "PDEquipmentManagerComponent.generated.h"

class UActorComponent;
class UPDAbilitySystemComponent;
class UPDEquipmentDefinition;
class UPDEquipmentInstance;
class UPDEquipmentManagerComponent;
class UObject;
struct FFrame;
struct FPDEquipmentList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FPDAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FPDAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend FPDEquipmentList;
	friend UPDEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UPDEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UPDEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FPDAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FPDEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FPDEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FPDEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FPDAppliedEquipmentEntry, FPDEquipmentList>(Entries, DeltaParms, *this);
	}

	UPDEquipmentInstance* AddEntry(TSubclassOf<UPDEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UPDEquipmentInstance* Instance);

private:
	UPDAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UPDEquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FPDAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FPDEquipmentList> : public TStructOpsTypeTraitsBase2<FPDEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};


/**
 * 
   실질적으로 폰에 적용되는 장비를 관리합니다
 */
UCLASS(BlueprintType, Const)
class UPDEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UPDEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "PD|EquipManager")
	static UPDEquipmentManagerComponent* FindEquipmentManagerComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UPDEquipmentManagerComponent>() : nullptr); }


	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UPDEquipmentInstance* EquipItem(TSubclassOf<UPDEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UPDEquipmentInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPDEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UPDEquipmentInstance> InstanceType);

 	/** Returns all equipped instances of a given type, or an empty array if none are found */
 	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UPDEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UPDEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FPDEquipmentList EquipmentList;
};
