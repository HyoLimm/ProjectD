// Copyright(c) Depsee. All Rights Reserved.

#pragma once


#include "PDTabListWidgetBase.h"
#include "Game/UI/Foundation/PDButtonBase.h"
#include "PDTabButtonBase.generated.h"

class UCommonLazyImage;
class UObject;
struct FFrame;
struct FSlateBrush;

UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
class PROJECTD_API UPDTabButtonBase : public UPDButtonBase, public IPDTabButtonInterface
{
	GENERATED_BODY()
public:

	void SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject);
	void SetIconBrush(const FSlateBrush& Brush);

protected:

	UFUNCTION()
	virtual void SetTabLabelInfo_Implementation(const FPDTabDescriptor& TabLabelInfo) override;

private:

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonLazyImage> LazyImage_Icon;
};
