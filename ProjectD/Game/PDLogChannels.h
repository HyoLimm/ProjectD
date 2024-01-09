// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

PROJECTD_API DECLARE_LOG_CATEGORY_EXTERN(LogPD, Log, All);
PROJECTD_API DECLARE_LOG_CATEGORY_EXTERN(LogPDExperience, Log, All);

PROJECTD_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
