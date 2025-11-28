// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "ItemAddInfoWidget.generated.h"

class UTextBlock;
struct FItemAddMessage;
/**
 * 
 */
UCLASS()
class SK_API UItemAddInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	FSKGameplayMessageListenerHandle ItemAddHandle;

	void OnAddItemMessageReceived(FGameplayTag Channel, const FItemAddMessage& Message);

	/** 팝업에 표시할 텍스트 */
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemText;

	/** 팝업 Fade 애니메이션 */
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* PopupFadeAnimation;
};
