// Copyright Epic Games, Inc. All Rights Reserved.

#include "PDTabButtonBase.h"

#include "CommonLazyImage.h"
#include "Game/UI/Common/PDTabListWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PDTabButtonBase)

class UObject;
struct FSlateBrush;

void UPDTabButtonBase::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrushFromLazyDisplayAsset(LazyObject);
	}
}

void UPDTabButtonBase::SetIconBrush(const FSlateBrush& Brush)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrush(Brush);
	}
}

void UPDTabButtonBase::SetTabLabelInfo_Implementation(const FPDTabDescriptor& TabLabelInfo)
{
	SetButtonText(TabLabelInfo.TabText);
	SetIconBrush(TabLabelInfo.IconBrush);
}

