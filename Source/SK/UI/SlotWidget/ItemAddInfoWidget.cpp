// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/ItemAddInfoWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemAddInfoWidget::SettingViewItem(const FText& ItemName, int32 ItemCount, UTexture2D* InItemIcon)
{
	// UI 표시 업데이트
	if (ItemNameText)
	{
		ItemNameText->SetText(ItemName);
	}

	if (ItemCountText)
	{
		ItemCountText->SetText(FText::AsNumber(ItemCount));
	}

	if (ItemIconImage)
	{
		if (InItemIcon)
			ItemIconImage->SetBrushFromTexture(InItemIcon);
	}

	PlayViewAnimation();
}

void UItemAddInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UItemAddInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UItemAddInfoWidget::PlayViewAnimation()
{
	if (!PopupFadeAnimation)
	{
		return;
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);

	// 애니메이션 재생
	PlayAnimation(PopupFadeAnimation);

	// 애니메이션 길이 조회
	float Duration = PopupFadeAnimation->GetEndTime();

	// Duration 뒤에 Collapsed
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}),
		Duration,
		false
	);
}
