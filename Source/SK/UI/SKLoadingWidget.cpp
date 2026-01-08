// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SKLoadingWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameInstance/SKGameInstance.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void USKLoadingWidget::SetTargetProgress(float InProgress)
{
	TargetProgress = FMath::Clamp(InProgress, 0.f, 1.f);

	// 타이머가 안 돌고 있으면 시작
	if (!GetWorld()->GetTimerManager().IsTimerActive(ProgressTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(
			ProgressTimerHandle,
			this,
			&USKLoadingWidget::OnProgressTick,
			ProgressUpdateInterval,
			true
		);
	}
}

void USKLoadingWidget::SetLoadingText(const FText& InText)
{
	if (LoadingText)
	{
		LoadingText->SetText(InText);
	}
}

void USKLoadingWidget::OnProgressTick()
{
	DisplayedProgress = FMath::FInterpTo(
		DisplayedProgress,
		TargetProgress,
		ProgressUpdateInterval,
		ProgressInterpSpeed
	);

	if (ProgressBar)
	{
		ProgressBar->SetPercent(DisplayedProgress);
	}

	// 거의 도달
	if (FMath::IsNearlyEqual(DisplayedProgress, TargetProgress, 0.001f))
	{
		DisplayedProgress = TargetProgress;

		if (ProgressBar)
		{
			ProgressBar->SetPercent(DisplayedProgress);
		}

		GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);

		if (DisplayedProgress >= 1.0f && !bFadeOutPlayed)
		{
			PlayFadeOut();
		}
	}
}

void USKLoadingWidget::PlayFadeOut()
{
	bFadeOutPlayed = true;

	if (FadeOutAnim)
	{
		PlayAnimation(FadeOutAnim);
	}
	else
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (USKGameInstance* SKGI = Cast<USKGameInstance>(GI))
			{
				SKGI->ShowLoadingScreen(false);
			}
		}
	}
}

void USKLoadingWidget::OnFadeInFinished()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (USKGameInstance* SKGI = Cast<USKGameInstance>(GI))
		{
			SKGI->ShowLoadingScreen(false);
		}
	}
}

void USKLoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	DisplayedProgress = 0.f;
	SetTargetProgress(0.5f);
	
	if (UGameInstance* GI = GetGameInstance())
	{
		if (USKGameInstance* SKGI = Cast<USKGameInstance>(GI))
		{
			if (SKGI->RandomNormal == 1)
			{
				InitEndingLoadingVisual();
				SKGI->RandomNormal = 0;
			}
			else
			{
				InitRandomLoadingVisual();
			}
		}
	}
	
	USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this);
	if (!MessageSubsystem)
		return;
	
	StateChangeHandle = MessageSubsystem->RegisterListener<FLoadingStateMessage>(
		TAG_Message_Channel_LoadingInfo,
		this,
		&USKLoadingWidget::OnStateChangeReceived
	);

	if (FadeOutAnim)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &USKLoadingWidget::OnFadeInFinished);
		BindToAnimationFinished(FadeOutAnim, EndEvent);
	}	
}

void USKLoadingWidget::NativeDestruct()
{
	if (StateChangeHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(StateChangeHandle);
		}
	}
	
	Super::NativeDestruct();
}

void USKLoadingWidget::OnStateChangeReceived(FGameplayTag Channel, const FLoadingStateMessage& Message)
{
	SetTargetProgress(Message.Progress);

	if (!Message.MessageText.IsEmpty())
	{
		SetLoadingText(Message.MessageText);
	}
}

void USKLoadingWidget::InitRandomLoadingVisual()
{
	// ---------- Background Image ----------
	if (RandomLoadingImages.Num() > 0 && BackgroundImage)
	{
		const int32 ImageIndex = FMath::RandRange(0, RandomLoadingImages.Num() - 1);

		if (UTexture2D* Texture = RandomLoadingImages[ImageIndex])
		{
			BackgroundImage->SetBrushFromTexture(Texture, true);
		}
	}

	// ---------- Tooltip Text ----------
	if (RandomTooltipTexts.Num() > 0 && LoadingToolTipText)
	{
		const int32 TooltipIndex = FMath::RandRange(0, RandomTooltipTexts.Num() - 1);
		LoadingToolTipText->SetText(RandomTooltipTexts[TooltipIndex]);
	}
}

void USKLoadingWidget::InitEndingLoadingVisual()
{
	// ---------- Fixed Tooltip Text ----------
	if (LoadingToolTipText)
	{
		LoadingToolTipText->SetText(EndingTooltipTexts);
	}

	// ---------- Fixed Background Image ----------
	if (!EndingLoadingTexture)
	{
		LoadingToolTipText->SetText(FText());
		return;
	}

	if (BackgroundImage)
	{
		BackgroundImage->SetBrushFromTexture(EndingLoadingTexture, true);
	}
}
