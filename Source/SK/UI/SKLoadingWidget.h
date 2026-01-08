// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "SKLoadingWidget.generated.h"

class UImage;
struct FLoadingStateMessage;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class SK_API USKLoadingWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetTargetProgress(float InProgress);
	void SetLoadingText(const FText& InText);
	void OnProgressTick();

	void PlayFadeOut();
	
	UFUNCTION()
	void OnFadeInFinished();
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	FSKGameplayMessageListenerHandle StateChangeHandle;

	void OnStateChangeReceived(FGameplayTag Channel, const FLoadingStateMessage& Message);

	void InitRandomLoadingVisual();
	void InitEndingLoadingVisual();
	
	UPROPERTY(EditDefaultsOnly, Category="Loading")
	TArray<UTexture2D*> RandomLoadingImages;

	UPROPERTY(EditDefaultsOnly, Category="Loading")
	UTexture2D* EndingLoadingTexture;
	
	UPROPERTY(EditDefaultsOnly, Category = "Loading", meta = (MultiLine = true))
	TArray<FText> RandomTooltipTexts;

	UPROPERTY(EditDefaultsOnly, Category = "Loading", meta = (MultiLine = true))
	FText EndingTooltipTexts;
	
	UPROPERTY(meta = (BindWidget))
	UImage* BackgroundImage;
	
	// Progress
	float TargetProgress = 0.f;
	float DisplayedProgress = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Loading|Progress")
	float ProgressUpdateInterval = 0.02f;

	UPROPERTY(EditDefaultsOnly, Category="Loading|Progress")
	float ProgressInterpSpeed = 6.f;

	FTimerHandle ProgressTimerHandle;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoadingText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoadingToolTipText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutAnim;

	bool bFadeOutPlayed = false;
};
