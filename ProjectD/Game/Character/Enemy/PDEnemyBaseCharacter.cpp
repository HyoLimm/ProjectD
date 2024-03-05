// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Character/Enemy/PDEnemyBaseCharacter.h"
#include "Game/AbilitySystem/PDAbilitySystemComponent.h"
#include "Game/AbilitySystem/Attributes/PDHealthSet.h"
#include "Game/AbilitySystem/Attributes/PDCombatSet.h"

APDEnemyBaseCharacter::APDEnemyBaseCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	BehaviorComponent = CreateDefaultSubobject<UPDAIBehaviorComponent>(TEXT("AIBehaviorComponent"));


	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UPDAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));

	HealthSet = CreateDefaultSubobject<UPDHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UPDCombatSet>(TEXT("CombatSet"));
}

UAbilitySystemComponent* APDEnemyBaseCharacter::GetAbilitySystemComponent() const
{
	return GetPDAbilitySystemComponent();
}

void APDEnemyBaseCharacter::PostInitializeComponents()
{
	if (PawnExtComponent)
	{
		PawnExtComponent->InitializeAbilitySystem(AbilitySystemComponent,this);
	}

	Super::PostInitializeComponents();
}

void APDEnemyBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AddCharacterAbilites();
}

void APDEnemyBaseCharacter::UnPossessed()
{
	Super::UnPossessed();
}

void APDEnemyBaseCharacter::AddCharacterAbilites()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddCharacterAbilities(StartupAbilities);
	}
}

void APDEnemyBaseCharacter::OnDeathStarted(AActor* OwningActor)
{
	Super::OnDeathStarted(OwningActor);

	//스폰한다

}

void APDEnemyBaseCharacter::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();
}

void APDEnemyBaseCharacter::OnAbilitySystemUninitialized()
{
	Super::OnAbilitySystemUninitialized();
}
