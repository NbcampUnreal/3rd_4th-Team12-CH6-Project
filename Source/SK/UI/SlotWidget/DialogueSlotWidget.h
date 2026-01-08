// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "DialogueSlotWidget.generated.h"

class UTextBlock;
struct FDialogSkipMessage;
struct FDialogSendMessage;
/**
 * 
 */
UCLASS()
class SK_API UDialogueSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FSKGameplayMessageListenerHandle DialogueMessageHandle;

	void DialogueSettingMessageReceived(FGameplayTag Channel, const FDialogSendMessage& Message);

	FSKGameplayMessageListenerHandle DialogueSkipHandle;

	void DialogueSkipMessageReceived(FGameplayTag Channel, const FDialogSkipMessage& Message);
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void StartTyping(const FString& InText);
	void TypeNextChar();
	void SkipTyping();
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DialogueText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_CloseImage;

	FString FullText;
	int32 CurrentCharIndex = 0;

	FTimerHandle TypingTimerHandle;

	float TypingInterval = 0.07f; // 글자 속도

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bTyping = false;
};
