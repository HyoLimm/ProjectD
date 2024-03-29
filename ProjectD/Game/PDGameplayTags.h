// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

namespace PDGameplayTags
{
	PROJECTD_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// Declare all of the custom native tags that PD will use
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	//PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
	//PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
	//PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);

	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);

	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Experience);

	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_GodMode);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_UnlimitedHealth);

	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	// These are mappings from MovementMode enums to GameplayTags associated with those enums (below)
	PROJECTD_API	extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	PROJECTD_API	extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);

	PROJECTD_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
};
