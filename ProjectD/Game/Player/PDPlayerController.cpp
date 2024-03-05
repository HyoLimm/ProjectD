// Fill out your copyright notice in the Description page of Project Settings.


#include "PDPlayerController.h"
#include "CommonInputSubsystem.h"

#include "Game/Player/PDCheatManager.h"
#include "Game/Player/PDPlayerState.h"

namespace PD
{
	namespace Input
	{
		static int32 ShouldAlwaysPlayForceFeedback = 0;
		static FAutoConsoleVariableRef CVarShouldAlwaysPlayForceFeedback(TEXT("PDPC.ShouldAlwaysPlayForceFeedback"),
			ShouldAlwaysPlayForceFeedback,
			TEXT("Should force feedback effects be played, even if the last input device was not a gamepad?"));
	}
}



APDPlayerController::APDPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CheatClass = UPDCheatManager::StaticClass();
}

APDPlayerState* APDPlayerController::GetPDPlayerState() const
{
	return CastChecked<APDPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UPDAbilitySystemComponent* APDPlayerController::GetPDAbilitySystemComponent() const
{
	const APDPlayerState* PDPS = GetPDPlayerState();
	return (PDPS ? PDPS->GetPDAbilitySystemComponent() : nullptr);
}

void APDPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void APDPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

}

void APDPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	if (const UPDLocalPlayer* LocalPlayer = Cast<UPDLocalPlayer>(InPlayer))
	{
		UPDSettingsShared* UserSettings = LocalPlayer->GetSharedSettings();
		UserSettings->OnSettingChanged.AddUObject(this, &ThisClass::OnSettingsChanged);

		OnSettingsChanged(UserSettings);
	}
}

void APDPlayerController::AddCheats(bool bForce)
{
#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER
}

void APDPlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)
{
//������̳� �����е忡 ���� ���� �ǵ�� ����
	if (bForceFeedbackEnabled)
	{
		if (const UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetLocalPlayer()))
		{
			const ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();
			if (PD::Input::ShouldAlwaysPlayForceFeedback || CurrentInputType == ECommonInputType::Gamepad || CurrentInputType == ECommonInputType::Touch)
			{
				InputInterface->SetForceFeedbackChannelValues(ControllerId, ForceFeedbackValues);
				return;
			}
		}
	}

	InputInterface->SetForceFeedbackChannelValues(ControllerId, FForceFeedbackValues());
}

void APDPlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents)
{

}

void APDPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void APDPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	//�Է� ó�� ���� ASC ���� ����
	if (UPDAbilitySystemComponent* PDASC = GetPDAbilitySystemComponent())
	{
		PDASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void APDPlayerController::OnSettingsChanged(UPDSettingsShared* Settings)
{
	bForceFeedbackEnabled = Settings->GetForceFeedbackEnabled();
}
