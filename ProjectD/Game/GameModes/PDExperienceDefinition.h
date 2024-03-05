// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "PDExperienceDefinition.generated.h"

class UGameFeatureAction;
class UPDPawnData;
class UPDExperienceActionSet;

/**
 * Definition of an experience
 */
UCLASS(BlueprintType, Const)
class UPDExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPDExperienceDefinition();

	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	//~UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	//~End of UPrimaryDataAsset interface

public:
	// �� ������ Ȱ��ȭ�ϱ⸦ ���ϴ� ���� ��� �÷����� ���
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	/** �÷��̾ ���� ������ �⺻ �� Ŭ���� */
	//@TODO: Make soft?
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const UPDPawnData> DefaultPawnData;

	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	// �� ������ �ε�/Ȱ��ȭ/��Ȱ��ȭ/��ε�� �� ������ �۾� ���
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// List of additional action sets to compose into this experience
	// �� �������� ������ �߰� �۾� ��Ʈ ���
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TArray<TObjectPtr<UPDExperienceActionSet>> ActionSets;
};
