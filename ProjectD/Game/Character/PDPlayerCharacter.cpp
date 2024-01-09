// Fill out your copyright notice in the Description page of Project Settings.


#include "PDPlayerCharacter.h"

#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"

#include "Game/Character/Component/PDCharacterMovementComponent.h"
#include "Game/Character/Component/PDCameraComponent.h"
#include "Game/Character/Component/PDPawnExtensionComponent.h"
#include "Game/Combat/Component/PDPlayerCombatComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"


static FName NAME_PDCharacterCollisionProfile_Capsule(TEXT("PDPawnCapsule"));
static FName NAME_PDCharacterCollisionProfile_Mesh(TEXT("PDPawnMesh"));


APDPlayerCharacter::APDPlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPDCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_PDCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionProfileName(NAME_PDCharacterCollisionProfile_Mesh);



	CameraComponent = CreateDefaultSubobject<UPDCameraComponent>(TEXT("PDCameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));

	ExperienceComponent = CreateDefaultSubobject<UPDPlayerLevelManagerComponent>(TEXT("PDPlayerLevelManagerComponent"));

	CombatComponent = CreateDefaultSubobject<UPDPlayerCombatComponent>(TEXT("PDPlayerCombatComponent"));
}


void APDPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PawnExtComponent->SetupPlayerInputComponent();
}

void APDPlayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

void APDPlayerCharacter::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();
}

void APDPlayerCharacter::OnAbilitySystemUninitialized()
{
	Super::OnAbilitySystemUninitialized();
}
