#include "PDPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"

#include "GameFramework/Pawn.h"
#include "Game/PDGameplayTags.h"
#include "Game/AbilitySystem/PDAbilitySystemComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(PDPawnExtensionComponent)

const FName UPDPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");


UPDPawnExtensionComponent::UPDPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;


	PawnData = nullptr;
}


void UPDPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("PDPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UPDPawnExtensionComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one PDPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register with the init state system early, this will only work if this is a game world
	// �ʱ� ���� �ý��ۿ� ���� ����Ͻʽÿ�. �̰��� ���� ������ ��쿡�� �۵��մϴ�
	RegisterInitStateFeature();
}



void UPDPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for changes to all features
	// ��� ����� ���� ���� ���
	const bool bCallIfReached = false;
	const FGameplayTag EmptyTag = FGameplayTag();

	BindOnActorInitStateChanged(NAME_None, EmptyTag, bCallIfReached);

	//// Notifies state manager that we have spawned, then try rest of default initialization
	//// state manager ���� �����Ǿ����� �˸��� ������ �⺻ �ʱ�ȭ�� �õ�
	ensure(TryToChangeInitState(PDGameplayTags::InitState_Spawned));

	CheckDefaultInitialization();
}

void UPDPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}



bool UPDPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	if (!CurrentState.IsValid() && DesiredState == PDGameplayTags::InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		if (Pawn)
		{
			return true;
		}
	}
	if (CurrentState == PDGameplayTags::InitState_Spawned && DesiredState == PDGameplayTags::InitState_DataAvailable)
	{
		// Pawn data is required.
		if (!PawnData)
		{
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			// Check for being possessed by a controller.
			// ��Ʈ�ѷ��� �����Ǿ� �ִ��� Ȯ���մϴ�.
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == PDGameplayTags::InitState_DataAvailable && DesiredState == PDGameplayTags::InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		return Manager->HaveAllFeaturesReachedInitState(Pawn, PDGameplayTags::InitState_DataAvailable);
	}
	else if (CurrentState == PDGameplayTags::InitState_DataInitialized && DesiredState == PDGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UPDPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (DesiredState == PDGameplayTags::InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
		//�̴� ���� �� ���� ������ �����ϴ� �ٸ� ������Ʈ���� ��� ó���˴ϴ�
	}
}

void UPDPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// If another feature is now in DataAvailable, see if we should transition to DataInitialized
	// DataAvailable�� �ٸ� ����� �ִ� ��� DataInitialized�� ��ȯ�ؾ� �ϴ��� Ȯ��
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == PDGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}



void UPDPawnExtensionComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	// ���� ��Ȳ�� Ȯ���ϱ� ���� ������ �� �ִ� �ٸ� ����� ������ ���ʽÿ�
	CheckDefaultInitializationForImplementers();

	const TArray<FGameplayTag> StateChain = { PDGameplayTags::InitState_Spawned, PDGameplayTags::InitState_DataAvailable, PDGameplayTags::InitState_DataInitialized, PDGameplayTags::InitState_GameplayReady };



	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	// �̷��� �ϸ� ������ �ܰ�(BeginPlay������ ������)���� ���� �÷��� �غ� �� ������ ������ �ʱ�ȭ �ܰ踦 �����մϴ�
	ContinueInitStateChain(StateChain);
}

void UPDPawnExtensionComponent::SetPawnData(const UPDPawnData* InPawnData)
{
	check(InPawnData);

	APawn* Pawn = GetPawnChecked<APawn>();
	if (PawnData)
	{
		//UE_LOG(LogPD, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	//Pawn->ForceNetUpdate();

	CheckDefaultInitialization();
}




void UPDPawnExtensionComponent::InitializeAbilitySystem(UPDAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		// The ability system component hasn't changed.
		return;
	}

	if (AbilitySystemComponent)
	{
		// Clean up the old ability system component.
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	//UE_LOG(Log, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
		//UE_LOG(Log, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		// There is already a pawn acting as the ASC's avatar, so we need to kick it out
		// This can happen on clients if they're lagged: their new pawn is spawned + possessed before the dead one is removed
		ensure(!ExistingAvatar->HasAuthority());

		if (UPDPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	if (ensure(PawnData))
	{
		//InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	OnAbilitySystemInitialized.Broadcast();
}

void UPDPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void UPDPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(PDGameplayTags::Ability_Behavior_SurvivesDeath);
	
		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UPDPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UPDPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}
