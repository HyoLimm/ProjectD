// Fill out your copyright notice in the Description page of Project Settings.


#include "PDBaseCharacter.h"

#include "PDCharacterMovementComponent.h"
#include "PDPawnExtensionComponent.h"
#include "PDHealthComponent.h"
#include <Components/CapsuleComponent.h>
#include "Game/PDGameplayTags.h"
#include "Game/AbilitySystem/PDAbilitySystemComponent.h"
#include "Game/AbilitySystem/PDAbilitySystemComponent.h"

APDBaseCharacter::APDBaseCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPDCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;


	PawnExtComponent = CreateDefaultSubobject<UPDPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));


	HealthComponent = CreateDefaultSubobject<UPDHealthComponent>(TEXT("HealthComponent"));
}

UPDAbilitySystemComponent* APDBaseCharacter::GetPDAbilitySystemComponent() const
{
	return PawnExtComponent->GetPDAbilitySystemComponent();
}

void APDBaseCharacter::OnAbilitySystemInitialized()
{
	UPDAbilitySystemComponent* PDASC = GetPDAbilitySystemComponent();
	check(PDASC);

	HealthComponent->InitializeWithAbilitySystem(PDASC);

	InitializeGameplayTags();
}

void APDBaseCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
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

void APDBaseCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void APDBaseCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
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

		UPDCharacterMovementComponent* PDMoveComp = CastChecked<UPDCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(PDMoveComp->MovementMode, PDMoveComp->CustomMovementMode, true);
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

