// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Character/PDBaseCharacter.h"

#include "Game/Character/Component/PDCharacterMovementComponent.h"
#include "Game/Character/Component/PDPawnExtensionComponent.h"
#include "Game/Character/Component/PDHealthComponent.h"
#include <Components/CapsuleComponent.h>
#include "Game/PDGameplayTags.h"



APDBaseCharacter::APDBaseCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPDCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;


	PawnExtComponent = CreateDefaultSubobject<UPDPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));


	HealthComponent = CreateDefaultSubobject<UPDHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
}

UPDAbilitySystemComponent* APDBaseCharacter::GetPDAbilitySystemComponent() const
{
	return Cast<UPDAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* APDBaseCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetPDAbilitySystemComponent();
}

bool APDBaseCharacter::IsAlive()
{
	if (HealthComponent)
	{
		return HealthComponent->IsAlive();
	}

	check(false);
	return false;
}

void APDBaseCharacter::OnAbilitySystemInitialized()
{
	UPDAbilitySystemComponent* LyraASC = GetPDAbilitySystemComponent();
	check(LyraASC);

	HealthComponent->InitializeWithAbilitySystem(LyraASC);


	InitializeGameplayTags();
}

void APDBaseCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void APDBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();
}

void APDBaseCharacter::UnPossessed()
{
	AController* const OldController = Controller;

	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();
}

// Called when the game starts or when spawned
void APDBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void APDBaseCharacter::OnDeathStarted(AActor* OwningActor)
{
	DisableMovementAndCollision();
}

void APDBaseCharacter::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void APDBaseCharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UPDCharacterMovementComponent* PDMoveComp = CastChecked<UPDCharacterMovementComponent>(GetCharacterMovement());
	PDMoveComp->StopMovementImmediately();
	PDMoveComp->DisableMovement();
}

void APDBaseCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}

void APDBaseCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UPDAbilitySystemComponent* ASC = GetPDAbilitySystemComponent())
	{
		if (ASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void APDBaseCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void APDBaseCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	// 이전 폰에서 ability system에 남아 있을 수 있는 태그를 지웁니다.
	if (UPDAbilitySystemComponent* ASC = GetPDAbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : PDGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				ASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : PDGameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				ASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		UPDCharacterMovementComponent* LyraMoveComp = CastChecked<UPDCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(LyraMoveComp->MovementMode, LyraMoveComp->CustomMovementMode, true);
	}
}

void APDBaseCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (UPDAbilitySystemComponent* ASC = GetPDAbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = PDGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = PDGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			ASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

// Called every frame
void APDBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APDBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

