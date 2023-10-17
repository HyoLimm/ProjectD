// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/System/PDGameInstance.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Game/PDGameplayTags.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(PDGameInstance)


UPDGameInstance::UPDGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UPDGameInstance::Init()
{
	Super::Init();

	// Register our custom init states
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{	
		const bool bAddBefore = false;
		ComponentManager->RegisterInitState(PDGameplayTags::InitState_Spawned, bAddBefore, FGameplayTag());
		ComponentManager->RegisterInitState(PDGameplayTags::InitState_DataAvailable, bAddBefore, PDGameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(PDGameplayTags::InitState_DataInitialized, bAddBefore, PDGameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(PDGameplayTags::InitState_GameplayReady, bAddBefore, PDGameplayTags::InitState_DataInitialized);
	}
}


void UPDGameInstance::Shutdown()
{
	Super::Shutdown();
}
