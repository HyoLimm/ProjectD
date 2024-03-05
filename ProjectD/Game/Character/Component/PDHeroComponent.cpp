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
		//	init state 시스템에 조기에 등록하십시오. game world 인 경우에만 작동한다.
		RegisterInitStateFeature();
	}
}

void UPDHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	// pawn extension component가 init 상태로 변경될 때 듣기
	const bool bCallIfReached = false;
	const FGameplayTag EmptyTag = FGameplayTag();
	BindOnActorInitStateChanged(UPDPawnExtensionComponent::NAME_ActorFeatureName, EmptyTag, bCallIfReached);

	// 스폰이 완료되었음을 알리고  나머지 초기화 진행
	ensure(TryToChangeInitState(PDGameplayTags::InitState_Spawned));

	//나머지 초기화를 시도합니다
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
		// 진짜 폰이 이미 있으면 리턴
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == PDGameplayTags::InitState_Spawned && DesiredState == PDGameplayTags::InitState_DataAvailable)
	{
		// 플레이어 스테이트는 필수.
		if (false == GetPlayerState<APDPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		// 권한이 있거나, 자율적인 경우에는 등록된 플레이어 스테이트의 소유권을 가진 컨트롤러를 기다려야 한다.
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
			// 로컬 제어시, InputComponent 그리고 Local플레이어는 필수다.
			if (!Pawn->InputComponent || !PDPC || !PDPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == PDGameplayTags::InitState_DataAvailable && DesiredState == PDGameplayTags::InitState_DataInitialized)
	{
		// 플레이어 스테이트 및 Extention 컴포넌트 대기
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

				//  Player State는 이 플레이어의 영구 데이터(사망자 및 여러 폰에 걸쳐 지속되는 상태)를 보유합니다.
				//  ability system component 및 attribute sets는 player state에 따라 라이브로 제공됩니다.
				PawnExtComp->InitializeAbilitySystem(PDPlayerState->GetPDAbilitySystemComponent(), PDPlayerState);
			}


			// InputComponent 초기화
			if (APDPlayerController* PDPC = GetController<APDPlayerController>())
			{
				if (Pawn->InputComponent != nullptr)
				{
					InitializePlayerInput(Pawn->InputComponent);
				}
			}

			const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
			///카메라 변화감지
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
			//extension component에 다른 모든 컴포넌트가 초기화되었다고 표시되면 다음 상태로 진행합니다
			CheckDefaultInitialization();
		}
	}
}

//static const TArray<FGameplayTag> StateChain = { PDGameplayTags::InitState_Spawned, PDGameplayTags::InitState_DataAvailable, PDGameplayTags::InitState_DataInitialized, PDGameplayTags::InitState_GameplayReady };

void UPDHeroComponent::CheckDefaultInitialization()
{
	const TArray<FGameplayTag> StateChain = { PDGameplayTags::InitState_Spawned, PDGameplayTags::InitState_DataAvailable, PDGameplayTags::InitState_DataInitialized, PDGameplayTags::InitState_GameplayReady };
	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	//이렇게 하면 생성된 단계(BeginPlay에서만 설정됨)에서 게임 플레이 준비가 될 때까지 데이터 초기화 단계를 진행합니다
	ContinueInitStateChain(StateChain);
}

void UPDHeroComponent::AddAdditionalInputConfig(const UPDInputConfig* InputConfig)
{
	//외부에서 inputconfig 추가할 경우 지금은 UGameFeatureAction_AddInputBinding 에서 호출함
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
		//예기치 않은 Input Component 클래스입니다! Gameplay Abilities 은 입력에 구속되지 않습니다.	input component를 UPDInputComponent 또는 그 하위 클래스로 변경합니다.
		if (ensureMsgf(PDIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UPDInputComponent or a subclass of it.")))
		{
			TArray<uint32> BindHandles;
			PDIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void UPDHeroComponent::RemoveAdditionalInputConfig(const UPDInputConfig* InputConfig)
{
	//구현해야함
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

				//1. 멤버변수 매핑추가
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
							// 실제로 로컬 플레이어에 config 추가					
							Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
						}
					}
				}

				//2. 파라미터 입력컴포넌트 바인딩
				if (UPDInputComponent* PDInputComponent = Cast<UPDInputComponent>(PlayerInputComponent))
				{
					//플레이어가 설정했을 수 있는 키 매핑을 추가합니다
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

