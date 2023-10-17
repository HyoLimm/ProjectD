// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDBaseCharacter.h"
#include "InputActionValue.h"
#include "PDPlayerCharacter.generated.h"

class UPDCameraComponent;
class UPDPawnExtensionComponent;
class UPDHealthComponent;

UCLASS()
class PROJECTD_API APDPlayerCharacter : public APDBaseCharacter
{
	GENERATED_BODY()
public:
	APDPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PD|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPDCameraComponent> CameraComponent;




public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();
};
