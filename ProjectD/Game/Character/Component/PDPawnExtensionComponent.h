// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "PDPawnExtensionComponent.generated.h"

class UPDPawnData;



UCLASS()
class PROJECTD_API UPDPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UPDPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);


	/** The name of this overall feature, this one depends on the other named component features */
	//이 전체 기능의 이름, 이 기능은 다른 명명된 구성 요소 기능에 따라 달라집니다
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface


	UFUNCTION(BlueprintPure, Category = "PD|Pawn")
	static UPDPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UPDPawnExtensionComponent>() : nullptr); }

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	/** Sets the current pawn data */
	void SetPawnData(const UPDPawnData* InPawnData);



	///** Gets the current ability system component, which may be owned by a different actor */
	///** 다른 액터가 소유할 수 있는 현재 ability system component를 가져옵니다*/
	//UFUNCTION(BlueprintPure, Category = "PD|Pawn")
	//UPDAbilitySystemComponent* GetPDAbilitySystemComponent() const { return AbilitySystemComponent; }


public:


	/** Should be called by the owning pawn to become the avatar of the ability system. */
	void InitializeAbilitySystem(UPDAbilitySystemComponent* InASC, AActor* InOwnerActor);


	/** Should be called by the owning pawn when the input component is setup. */
	void SetupPlayerInputComponent();


	/** Should be called by the owning pawn to remove itself as the avatar of the ability system. */
	void UninitializeAbilitySystem();


	/** Register with the OnAbilitySystemInitialized delegate and broadcast if our pawn has been registered with the ability system component */
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	/** Register with the OnAbilitySystemUninitialized delegate fired when our pawn is removed as the ability system's avatar actor */
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

protected:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:	
	/** Delegate fired when our pawn becomes the ability system's avatar actor */
	FSimpleMulticastDelegate OnAbilitySystemInitialized;


	/** Delegate fired when our pawn is removed as the ability system's avatar actor */
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;


	UPROPERTY(EditInstanceOnly, Category = "PD|Pawn")
	TObjectPtr<const UPDPawnData> PawnData;

	///** Pointer to the ability system component that is cached for convenience. */
	//UPROPERTY()
	//TObjectPtr<UPDAbilitySystemComponent> AbilitySystemComponent;
};
