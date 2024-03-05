// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDExperienceManager.h"
#include "Game/GameModes/PDExperienceManager.h"
#include "Engine/Engine.h"
#include "Subsystems/SubsystemCollection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDExperienceManager)

#if WITH_EDITOR

void UPDExperienceManager::OnPlayInEditorBegun()
{
	ensure(GameFeaturePluginRequestCountMap.IsEmpty());
	GameFeaturePluginRequestCountMap.Empty();
}

void UPDExperienceManager::NotifyOfPluginActivation(const FString PluginURL)
{
	if (GIsEditor)
	{
		UPDExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UPDExperienceManager>();
		check(ExperienceManagerSubsystem);

		// Track the number of requesters who activate this plugin. Multiple load/activation requests are always allowed because concurrent requests are handled.
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);
		++Count;
	}
}

bool UPDExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)
{
	if (GIsEditor)
	{
		UPDExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UPDExperienceManager>();
		check(ExperienceManagerSubsystem);

		// Only let the last requester to get this far deactivate the plugin
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindChecked(PluginURL);
		--Count;

		if (Count == 0)
		{
			ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.Remove(PluginURL);
			return true;
		}

		return false;
	}

	return true;
}

#endif
