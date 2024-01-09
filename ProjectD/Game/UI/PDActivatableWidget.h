// Copyright(c) Depsee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PDActivatableWidget.generated.h"

struct FUIInputConfig;

UENUM(BlueprintType)
enum class EPDWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

// An activatable widget that automatically drives the desired input config when activated
UCLASS(Abstract, Blueprintable)
class PROJECTD_API UPDActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UPDActivatableWidget(const FObjectInitializer& ObjectInitializer);

public:

	//~UCommonActivatableWidget interface
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~End of UCommonActivatableWidget interface

#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif

protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	// 이 UI가 활성화되는 동안 사용할 원하는 입력 모드, 예를 들어 키를 눌러 게임/플레이어 컨트롤러에 계속 도달하시겠습니까?
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EPDWidgetInputMode InputConfig = EPDWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
