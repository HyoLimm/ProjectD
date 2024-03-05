// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "AbilitySystemInterface.h"
#include "PDBaseCharacter.generated.h"


class UPDPawnExtensionComponent;
class UPDHealthComponent;
class UPDAbilitySystemComponent;
class UAbilitySystemComponent;


UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class PROJECTD_API APDBaseCharacter : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:	
	APDBaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "PD|Character")
	UPDAbilitySystemComponent* GetPDAbilitySystemComponent() const;

	//~IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface interface


	bool		IsAlive();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PD|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDPawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PD|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDHealthComponent>		  HealthComponent;
protected:
	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);


	void		DisableMovementAndCollision();
	void		DestroyDueToDeath();
	void		UninitAndDestroy();


	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDeathFinished"))
	void K2_OnDeathFinished();


	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;



	void		InitializeGameplayTags();

	void		SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
