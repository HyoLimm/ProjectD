// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AbilitySystem/Attributes/PDAttributeSet.h"


UPDAttributeSet::UPDAttributeSet()
{

}

UWorld* UPDAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}
