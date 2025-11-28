// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/ItemAddInfoWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemAddInfoWidget::SettingViewItem(const FText& ItemName, int32 ItemCount, UTexture2D* InItemIcon)
{
	UE_LOG(LogTemp, Warning, TEXT("[ItemAddInfoWidget] SettingViewItem 호출됨"));
	UE_LOG(LogTemp, Warning, TEXT(" - Name: %s"), *ItemName.ToString());
	UE_LOG(LogTemp, Warning, TEXT(" - Count: %d"), ItemCount);
	UE_LOG(LogTemp, Warning, TEXT(" - Icon: %s"), InItemIcon ? *InItemIcon->GetName() : TEXT("None"));

	// UI 표시 업데이트
	if (ItemNameText)
	{
		ItemNameText->SetText(ItemName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemNameText 바인딩 안됨"));
	}

	if (ItemCountText)
	{
		ItemCountText->SetText(FText::AsNumber(ItemCount));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemCountText 바인딩 안됨"));
	}

	if (ItemIconImage)
	{
		if (InItemIcon)
			ItemIconImage->SetBrushFromTexture(InItemIcon);
		else
			UE_LOG(LogTemp, Warning, TEXT("InItemIcon NULL!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemIconImage 바인딩 안됨"));
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
	UE_LOG(LogTemp, Warning, TEXT("[ItemAddInfoWidget] PlayViewAnimation 호출됨"));

	if (!PopupFadeAnimation)
	{
		UE_LOG(LogTemp, Error, TEXT("PopupFadeAnimation 없음! UMG에서 애니메이션 바인딩 확인 필요"));
		return;
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);

	// 애니메이션 재생
	FWidgetAnimationHandle Handle = PlayAnimation(PopupFadeAnimation);

	if (Handle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayAnimation 성공 (Handle Valid)"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayAnimation 실패 (Handle Invalid)"));
	}


	// 애니메이션 길이 조회
	float Duration = PopupFadeAnimation->GetEndTime();
	UE_LOG(LogTemp, Warning, TEXT("PopupFadeAnimation Duration = %f"), Duration);

	if (Duration <= 0.f)
	{
		UE_LOG(LogTemp, Error, TEXT("애니메이션 Duration이 0 이하입니다. UMG 애니메이션 길이 확인!"));
	}

	// Duration 뒤에 Collapsed
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			UE_LOG(LogTemp, Warning, TEXT("[ItemAddInfoWidget] Collapsed로 변경됨"));
			SetVisibility(ESlateVisibility::Collapsed);
		}),
		Duration,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("Timer 설정 완료: %f초 뒤 Collapsed"), Duration);
}
