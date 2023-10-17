// Fill out your copyright notice in the Description page of Project Settings.


#include "PDSettingsLocal.h"

UPDSettingsLocal::UPDSettingsLocal()
{

}

UPDSettingsLocal* UPDSettingsLocal::Get()
{
	return GEngine ? CastChecked<UPDSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}
