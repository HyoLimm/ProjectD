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

}

void APDBaseCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{

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

