// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"

#include "Game/GameFeatures/GameFeatureAction_AddInputContextMapping.h"
#include "PDHeroComponent.generated.h"

class UPDCameraMode;
struct FMappableConfigPair;
struct FInputActionValue;

//
// �÷��̾� Controllered Pawn(�Ǵ� AI)�� ���� �Է�(Input)�� ī�޶� ó��(Cam)�� �����ϴ� ������Ʈ�Դϴ�.
//  �̴� �ʱ�ȭ�� �����ϱ� ���� PDPawnExtensionComponent �� ���� �޶�����.
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class PROJECTD_API UPDHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:

	UPDHeroComponent(const FObjectInitializer& ObjectInitializer);


	/** The name of the extension event sent via UGameFrameworkComponentManager when ability inputs are ready to bind */
	static const FName NAME_BindInputsNow;

	/** The name of this component-implemented feature */
	static const FName NAME_ActorFeatureName;



	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface


		/** Adds mode-specific input config */
	void AddAdditionalInputConfig(const UPDInputConfig* InputConfig);

	/** Removes a mode-specific input config if it has been added */
	void RemoveAdditionalInputConfig(const UPDInputConfig* InputConfig);


	/** True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added */
	bool IsReadyToBindInputs() const;
protected:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);


	TSubclassOf<UPDCameraMode> DetermineCameraMode() const;

protected:
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);


protected:

	UPROPERTY(EditAnywhere)
	TArray<FInputMappingContextAndPriority> _DefaultInputMappings;


	/** ��ɿ� ���� ������ ī�޶� ��� */
	UPROPERTY()
	TSubclassOf<UPDCameraMode> AbilityCameraMode;

	/** True when player input bindings have been applied, will never be true for non - players */
	bool bReadyToBindInputs;
};
