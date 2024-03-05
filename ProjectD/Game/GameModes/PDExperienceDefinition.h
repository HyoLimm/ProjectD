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
	// 이 경험이 활성화하기를 원하는 게임 기능 플러그인 목록
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	/** 플레이어에 대해 생성할 기본 폰 클래스 */
	//@TODO: Make soft?
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const UPDPawnData> DefaultPawnData;

	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	// 이 경험이 로드/활성화/비활성화/언로드될 때 수행할 작업 목록
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// List of additional action sets to compose into this experience
	// 이 경험으로 구성할 추가 작업 세트 목록
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TArray<TObjectPtr<UPDExperienceActionSet>> ActionSets;
};
