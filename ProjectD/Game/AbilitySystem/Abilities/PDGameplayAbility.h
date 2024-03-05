// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbility.h"

#include "PDGameplayAbility.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpec;
struct FGameplayAbilitySpecHandle;

class AActor;
class AController;
class APDBaseCharacter;
class APDPlayerController;
class APlayerController;
class FText;
class IPDAbilitySourceInterface;
class UAnimMontage;
class UPDAbilityCost;
class UPDAbilitySystemComponent;
class UPDCameraMode;
class UPDHeroComponent;
class UObject;
struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayEffectSpec;
struct FGameplayEventData;

/**
 * EPDAbilityActivationPolicy
 *
 *	Defines how an ability is meant to activate.
 */
UENUM(BlueprintType)
enum class EPDAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};


/**
 * EPDAbilityActivationGroup
 *
 *	Defines how an ability activates in relation to other abilities.
 *  다른 abilities과 관련하여 ability이 활성화되는 방법을 정의합니다.
 */
UENUM(BlueprintType)
enum class EPDAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	// 능력은 다른 모든 능력과 독립적으로 실행됩니다.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	//능력은 취소되고 다른 독점(exclusive) 능력으로 대체됩니다.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	//능력은 다른 모든 독점적인 능력이 활성화되는 것을 막습니다.
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
};

/** Failure reason that can be used to play an animation montage when a failure occurs */
//  장애가 발생했을 때 애니메이션 몽타주를 재생하는 데 사용할 수 있는 장애 이유입니다.
USTRUCT(BlueprintType)
struct FPDAbilityMontageFailureMessage
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	// All the reasons why this ability has failed
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer FailureTags;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAnimMontage> FailureMontage = nullptr;
};

/**
 * UPDGameplayAbility
 *
 *	The base gameplay ability class used by this project.
 * //이 프로젝트에서 사용하는 기본 gameplay ability 클래스입니다.
 */
UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "이 프로젝트에서 사용하는 기본 gameplay ability 클래스입니다!"))
class PROJECTD_API UPDGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UPDAbilitySystemComponent;

public:

	UPDGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "PD|Ability")
	UPDAbilitySystemComponent* GetPDAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "PD|Ability")
	APDPlayerController* GetPDPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "PD|Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "PD|Ability")
	APDBaseCharacter* GetPDCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "PD|Ability")
	UPDHeroComponent* GetHeroComponentFromActorInfo() const;

	EPDAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	EPDAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	// Returns true if the requested activation group is a valid transition.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "PD|Ability", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool CanChangeActivationGroup(EPDAbilityActivationGroup NewGroup) const;

	// Tries to change the activation group.  Returns true if it successfully changed.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "PD|Ability", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool ChangeActivationGroup(EPDAbilityActivationGroup NewGroup);

	// Sets the ability's camera mode.
	UFUNCTION(BlueprintCallable, Category = "PD|Ability")
	void SetCameraMode(TSubclassOf<UPDCameraMode> CameraMode);

	// Clears the ability's camera mode.  Automatically called if needed when the ability ends.
	UFUNCTION(BlueprintCallable, Category = "PD|Ability")
	void ClearCameraMode();

	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
	{
		NativeOnAbilityFailedToActivate(FailedReason);
		ScriptOnAbilityFailedToActivate(FailedReason);
	}

protected:

	// Called when the ability fails to activate
	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	// Called when the ability fails to activate
	UFUNCTION(BlueprintImplementableEvent)
	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	//~UGameplayAbility interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const override;
	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	//~End of UGameplayAbility interface

	virtual void OnPawnAvatarSet();

	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel, const IPDAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const;

	/** Called when this ability is granted to the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	/** Called when this ability is removed from the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();

	/** Called when the ability system is initialized with a pawn avatar. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
	void K2_OnPawnAvatarSet();

protected:

	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PD|Ability Activation")
	EPDAbilityActivationPolicy ActivationPolicy;

	// Defines the relationship between this ability activating and other abilities activating.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PD|Ability Activation")
	EPDAbilityActivationGroup ActivationGroup;

	// Additional costs that must be paid to activate this ability
	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs)
	TArray<TObjectPtr<UPDAbilityCost>> AdditionalCosts;

	// Map of failure tags to simple error messages
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	TMap<FGameplayTag, FText> FailureTagToUserFacingMessages;

	// Map of failure tags to anim montages that should be played with them
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> FailureTagToAnimMontage;

	// If true, extra information should be logged when this ability is canceled. This is temporary, used for tracking a bug.
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	bool bLogCancelation;

	// Current camera mode set by the ability.
	TSubclassOf<UPDCameraMode> ActiveCameraMode;
};
