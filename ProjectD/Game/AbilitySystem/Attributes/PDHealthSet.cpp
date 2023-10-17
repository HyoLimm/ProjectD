// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AbilitySystem/Attributes/PDHealthSet.h"

UPDHealthSet::UPDHealthSet()
	: Health(100.0f)
	, MaxHealth(100.0f)
{
	bOutOfHealth = false;
}