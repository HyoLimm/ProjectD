// Fill out your copyright notice in the Description page of Project Settings.


#include "PDHeroComponent.h"

#include "PlayerMappableInputConfig.h"


#include "PDPawnExtensionComponent.h"

#include "Game/PDGameplayTags.h"
#include "Game/Character/PDPawnData.h"
#include "Game/Character/PDPlayerCharacter.h"
#include "Game/Player/PDLocalPlayer.h"
#include "Game/Input/PDInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Components/GameFrameworkComponentManager.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR
#include "Game/Player/PDPlayerController.h"
#include "Game/Player/PDPlayerState.h"
#include "PDCameraComponent.h"

#include <GameFramework/Pawn.h>
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Game/GameFeatures/GameFeatureAction_AddInputContextMapping.h"
#include "InputMappingContext.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(PDHeroComponent)



const FName UPDHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UPDHeroComponent::NAME_ActorFeatureName("Hero");



UPDHeroComponent::UPDHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	_bReadyToBindInputs = false;
}

void UPDHeroComponent::SetAbilityCameraMode(TSubclassOf<UPDCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		_AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void UPDHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		_AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}

void UPDHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		//UE_LOG(LogPD, Error, TEXT("[UPDHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("PDHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("PDHeroComponent");

			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));

			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		//	init state �ý��ۿ� ���⿡ ����Ͻʽÿ�. game world �� ��쿡�� �۵��Ѵ�.
		RegisterInitStateFeature();
	}
}

void UPDHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	// pawn extension component�� init ���·� ����� �� ���
	const bool bCallIfReached = false;
	const FGameplayTag EmptyTag = FGameplayTag();
	BindOnActorInitStateChanged(UPDPawnExtensionComponent::NAME_ActorFeatureName, EmptyTag, bCallIfReached);

	// ������ �Ϸ�Ǿ����� �˸���  ������ �ʱ�ȭ ����
	ensure(TryToChangeInitState(PDGameplayTags::InitState_Spawned));

	//������ �ʱ�ȭ�� �õ��մϴ�
	CheckDefaultInitialization();
}

void UPDHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

}

bool UPDHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);
	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == PDGameplayTags::InitState_Spawned)
	{
		// ��¥ ���� �̹� ������ ����
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == PDGameplayTags::InitState_Spawned && DesiredState == PDGameplayTags::InitState_DataAvailable)
	{
		// �÷��̾� ������Ʈ�� �ʼ�.
		if (false == GetPlayerState<APDPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		// ������ �ְų�, �������� ��쿡�� ��ϵ� �÷��̾� ������Ʈ�� �������� ���� ��Ʈ�ѷ��� ��ٷ��� �Ѵ�.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			APDPlayerController* PDPC = GetController<APDPlayerController>();

			// The input component and local player is required when locally controlled.
			// ���� �����, InputComponent �׸��� Local�÷��̾�� �ʼ���.
			if (!Pawn->InputComponent || !PDPC || !PDPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == PDGameplayTags::InitState_DataAvailable && DesiredState == PDGameplayTags::InitState_DataInitialized)
	{
		// �÷��̾� ������Ʈ �� Extention ������Ʈ ���
		APDPlayerState* PdPS = GetPlayerState<APDPlayerState>();

		return PdPS && Manager->HasFeatureReachedInitState(Pawn, UPDPawnExtensionComponent::NAME_ActorFeatureName, PDGameplayTags::InitState_DataInitialized);
	}
	else if (CurrentState == PDGameplayTags::InitState_DataInitialized && DesiredState == PDGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}



void UPDHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == PDGameplayTags::InitState_DataAvailable && DesiredState == PDGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		APDPlayerState* PDPlayerState = GetPlayerState<APDPlayerState>();
		if (Pawn && PDPlayerState)
		{
			const UPDPawnData* PawnData = nullptr;			
			if (UPDPawnExtensionComponent* PawnExtComp = UPDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
			{
				PawnData = PawnExtComp->GetPawnData<UPDPawnData>();
				check(PawnData);

				//  Player State�� �� �÷��̾��� ���� ������(����� �� ���� ���� ���� ���ӵǴ� ����)�� �����մϴ�.
				//  ability system component �� attribute sets�� player state�� ���� ���̺�� �����˴ϴ�.
				PawnExtComp->InitializeAbilitySystem(PDPlayerState->GetPDAbilitySystemComponent(), PDPlayerState);
			}


			// InputComponent �ʱ�ȭ
			if (APDPlayerController* PDPC = GetController<APDPlayerController>())
			{
				if (Pawn->InputComponent != nullptr)
				{
					InitializePlayerInput(Pawn->InputComponent);
				}
			}

			const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
			///ī�޶� ��ȭ����
			if (bIsLocallyControlled && PawnData)
			{
				if (UPDCameraComponent* CameraComponent = UPDCameraComponent::FindCameraComponent(Pawn))
				{
					CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
				}
			}
		}		
	}
}

void UPDHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UPDPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == PDGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			//extension component�� �ٸ� ��� ������Ʈ�� �ʱ�ȭ�Ǿ��ٰ� ǥ�õǸ� ���� ���·� �����մϴ�
			CheckDefaultInitialization();
		}
	}
}

//static const TArray<FGameplayTag> StateChain = { PDGameplayTags::InitState_Spawned, PDGameplayTags::InitState_DataAvailable, PDGameplayTags::InitState_DataInitialized, PDGameplayTags::InitState_GameplayReady };

void UPDHeroComponent::CheckDefaultInitialization()
{
	const TArray<FGameplayTag> StateChain = { PDGameplayTags::InitState_Spawned, PDGameplayTags::InitState_DataAvailable, PDGameplayTags::InitState_DataInitialized, PDGameplayTags::InitState_GameplayReady };
	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	//�̷��� �ϸ� ������ �ܰ�(BeginPlay������ ������)���� ���� �÷��� �غ� �� ������ ������ �ʱ�ȭ �ܰ踦 �����մϴ�
	ContinueInitStateChain(StateChain);
}

void UPDHeroComponent::AddAdditionalInputConfig(const UPDInputConfig* InputConfig)
{
	//�ܺο��� inputconfig �߰��� ��� ������ UGameFeatureAction_AddInputBinding ���� ȣ����
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UPDPawnExtensionComponent* PawnExtComp = UPDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		UPDInputComponent* PDIC = Pawn->FindComponentByClass<UPDInputComponent>();
		//����ġ ���� Input Component Ŭ�����Դϴ�! Gameplay Abilities �� �Է¿� ���ӵ��� �ʽ��ϴ�.	input component�� UPDInputComponent �Ǵ� �� ���� Ŭ������ �����մϴ�.
		if (ensureMsgf(PDIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UPDInputComponent or a subclass of it.")))
		{
			TArray<uint32> BindHandles;
			PDIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void UPDHeroComponent::RemoveAdditionalInputConfig(const UPDInputConfig* InputConfig)
{
	//�����ؾ���
}

bool UPDHeroComponent::IsReadyToBindInputs() const
{
	return _bReadyToBindInputs;
}

void UPDHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);


	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const UPDLocalPlayer* LocalPlayer = Cast<UPDLocalPlayer>(PC->GetLocalPlayer());
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}



	if (const UPDPawnExtensionComponent* PawnExtComp = UPDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UPDPawnData* PawnData = PawnExtComp->GetPawnData<UPDPawnData>())
		{
			if (const UPDInputConfig* InputConfig = PawnData->InputConfig)
			{

				//1. ������� �����߰�
				for (const FInputMappingContextAndPriority Mapping : _DefaultInputMappings)
				{					
					if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
					{
						if (Mapping.bRegisterWithSettings)
						{
							if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
							{
								Settings->RegisterInputMappingContext(IMC); 
							}

							FModifyContextOptions Options = {};
							Options.bIgnoreAllPressedKeysUntilRelease = false;
							// ������ ���� �÷��̾ config �߰�					
							Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
						}
					}
				}

				//2. �Ķ���� �Է�������Ʈ ���ε�
				if (UPDInputComponent* PDInputComponent = Cast<UPDInputComponent>(PlayerInputComponent))
				{
					//�÷��̾ �������� �� �ִ� Ű ������ �߰��մϴ�
					PDInputComponent->AddInputMappings(InputConfig, Subsystem);

					TArray<uint32> BindHandles;
					PDInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);


					PDInputComponent->BindNativeAction(InputConfig, PDGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
					PDInputComponent->BindNativeAction(InputConfig, PDGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
				}
			}
		}
	}

	if (ensure(!_bReadyToBindInputs))
	{
		_bReadyToBindInputs = true;
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow); 


}

TSubclassOf<UPDCameraMode> UPDHeroComponent::DetermineCameraMode() const
{
	if (_AbilityCameraMode)
	{
		return _AbilityCameraMode;
	}


	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (UPDPawnExtensionComponent* PawnExtComp = UPDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UPDPawnData* PawnData = PawnExtComp->GetPawnData<UPDPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

void UPDHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UPDHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UPDHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UPDPawnExtensionComponent* PawnExtComp = UPDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UPDAbilitySystemComponent* PDASC = PawnExtComp->GetPDAbilitySystemComponent())
			{
				PDASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UPDHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{

	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UPDPawnExtensionComponent* PawnExtComp = UPDPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UPDAbilitySystemComponent* PDASC = PawnExtComp->GetPDAbilitySystemComponent())
			{
				PDASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}

