// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/Cosmetics/PDControllerComponent_CharacterParts.h"
#include "Game/Cosmetics/PDCharacterPartTypes.h"
#include "Game/Cosmetics/PDPawnComponent_CharacterParts.h"
#include "GameFramework/CheatManagerDefines.h"
#include "PDCosmeticDeveloperSettings.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDControllerComponent_CharacterParts)

//////////////////////////////////////////////////////////////////////

UPDControllerComponent_CharacterParts::UPDControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPDControllerComponent_CharacterParts::BeginPlay()
{
	Super::BeginPlay();

	// Listen for pawn possession changed events
	if (HasAuthority())
	{
		if (AController* OwningController = GetController<AController>())
		{
			OwningController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);

			if (APawn* ControlledPawn = GetPawn<APawn>())
			{
				OnPossessedPawnChanged(nullptr, ControlledPawn);
			}
		}

		ApplyDeveloperSettings();
	}
}

void UPDControllerComponent_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllCharacterParts();
	Super::EndPlay(EndPlayReason);
}

UPDPawnComponent_CharacterParts* UPDControllerComponent_CharacterParts::GetPawnCustomizer() const
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		return ControlledPawn->FindComponentByClass<UPDPawnComponent_CharacterParts>();
	}
	return nullptr;
}

void UPDControllerComponent_CharacterParts::AddCharacterPart(const FPDCharacterPart& NewPart)
{
	AddCharacterPartInternal(NewPart, ECharacterPartSource::Natural);
}

void UPDControllerComponent_CharacterParts::AddCharacterPartInternal(const FPDCharacterPart& NewPart, ECharacterPartSource Source)
{
	FPDControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();
	NewEntry.Part = NewPart;
	NewEntry.Source = Source;

	if (UPDPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		if (NewEntry.Source != ECharacterPartSource::NaturalSuppressedViaCheat)
		{
			NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
		}
	}

}

void UPDControllerComponent_CharacterParts::RemoveCharacterPart(const FPDCharacterPart& PartToRemove)
{
	for (auto EntryIt = CharacterParts.CreateIterator(); EntryIt; ++EntryIt)
	{
		if (FPDCharacterPart::AreEquivalentParts(EntryIt->Part, PartToRemove))
		{
			if (UPDPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
			{
				PawnCustomizer->RemoveCharacterPart(EntryIt->Handle);
			}

			EntryIt.RemoveCurrent();
			break;
		}
	}
}

void UPDControllerComponent_CharacterParts::RemoveAllCharacterParts()
{
	if (UPDPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		for (FPDControllerCharacterPartEntry& Entry : CharacterParts)
		{
			PawnCustomizer->RemoveCharacterPart(Entry.Handle);
		}
	}

	CharacterParts.Reset();
}

void UPDControllerComponent_CharacterParts::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Remove from the old pawn
	if (UPDPawnComponent_CharacterParts* OldCustomizer = OldPawn ? OldPawn->FindComponentByClass<UPDPawnComponent_CharacterParts>() : nullptr)
	{
		for (FPDControllerCharacterPartEntry& Entry : CharacterParts)
		{
			OldCustomizer->RemoveCharacterPart(Entry.Handle);
			Entry.Handle.Reset();
		}
	}

	// Apply to the new pawn
	if (UPDPawnComponent_CharacterParts* NewCustomizer = NewPawn ? NewPawn->FindComponentByClass<UPDPawnComponent_CharacterParts>() : nullptr)
	{
		for (FPDControllerCharacterPartEntry& Entry : CharacterParts)
		{
			// Don't readd if it's already there, this can get called with a null oldpawn
			if (!Entry.Handle.IsValid() && Entry.Source != ECharacterPartSource::NaturalSuppressedViaCheat)
			{
				Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
			}
		}
	}
}

void UPDControllerComponent_CharacterParts::ApplyDeveloperSettings()
{
#if UE_WITH_CHEAT_MANAGER
	const UPDCosmeticDeveloperSettings* Settings = GetDefault<UPDCosmeticDeveloperSettings>();

	// Suppress or unsuppress natural parts if needed
	const bool bSuppressNaturalParts = (Settings->CheatMode == ECosmeticCheatMode::ReplaceParts) && (Settings->CheatCosmeticCharacterParts.Num() > 0);
	SetSuppressionOnNaturalParts(bSuppressNaturalParts);

	// Remove anything added by developer settings and re-add it
	UPDPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();
	for (auto It = CharacterParts.CreateIterator(); It; ++It)
	{
		if (It->Source == ECharacterPartSource::AppliedViaDeveloperSettingsCheat)
		{
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(It->Handle);
			}
			It.RemoveCurrent();
		}
	}

	// Add new parts
	for (const FPDCharacterPart& PartDesc : Settings->CheatCosmeticCharacterParts)
	{
		AddCharacterPartInternal(PartDesc, ECharacterPartSource::AppliedViaDeveloperSettingsCheat);
	}
#endif
}


void UPDControllerComponent_CharacterParts::AddCheatPart(const FPDCharacterPart& NewPart, bool bSuppressNaturalParts)
{
#if UE_WITH_CHEAT_MANAGER
	SetSuppressionOnNaturalParts(bSuppressNaturalParts);
	AddCharacterPartInternal(NewPart, ECharacterPartSource::AppliedViaCheatManager);
#endif
}

void UPDControllerComponent_CharacterParts::ClearCheatParts()
{
#if UE_WITH_CHEAT_MANAGER
	UPDPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();

	// Remove anything added by cheat manager cheats
	for (auto It = CharacterParts.CreateIterator(); It; ++It)
	{
		if (It->Source == ECharacterPartSource::AppliedViaCheatManager)
		{
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(It->Handle);
			}
			It.RemoveCurrent();
		}
	}

	ApplyDeveloperSettings();
#endif
}

void UPDControllerComponent_CharacterParts::SetSuppressionOnNaturalParts(bool bSuppressed)
{
#if UE_WITH_CHEAT_MANAGER
	UPDPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();

	for (FPDControllerCharacterPartEntry& Entry : CharacterParts)
	{
		if ((Entry.Source == ECharacterPartSource::Natural) && bSuppressed)
		{
			// Suppress
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(Entry.Handle);
				Entry.Handle.Reset();
			}
			Entry.Source = ECharacterPartSource::NaturalSuppressedViaCheat;
		}
		else if ((Entry.Source == ECharacterPartSource::NaturalSuppressedViaCheat) && !bSuppressed)
		{
			// Unsuppress
			if (PawnCustomizer != nullptr)
			{
				Entry.Handle = PawnCustomizer->AddCharacterPart(Entry.Part);
			}
			Entry.Source = ECharacterPartSource::Natural;
		}
	}
#endif
}

