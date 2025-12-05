// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/BuffDebuffIconWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "PlayerState/SKPlayerState.h"

void UBuffDebuffIconWidget::SetupIcon(const FBuffDebuffIconDataTable& Data, const FModifiedAttributeInfo& Info,
                                      float Duration, int32 StackCount, bool bSettingIsBuff)
{
	// ----------------------------
	// 1) 스탯 아이콘 적용
	// ----------------------------
	if (StatIcon && Data.StatIcon)
	{
		StatIcon->SetBrushFromTexture(Data.StatIcon);
	}

	// ----------------------------
	// 2) 증가/감소 아이콘 적용
	// ----------------------------
	if (ChangeIcon && Data.ModifierIcon)
	{
		ChangeIcon->SetBrushFromTexture(Data.ModifierIcon);
	}

	// ----------------------------
	// 3) 수치 텍스트 (+10%, -5 등)
	// Magnitude는 Info.Magnitude 사용
	// ----------------------------
	if (ValueText)
	{
		FString ValueString;

		float DisplayValue = FMath::Abs(Info.Magnitude);
		
		switch (Info.Op)
		{
		case EGameplayModOp::Additive:
			// +면 Buff, -면 Debuff
			ValueString = FString::Printf(TEXT("%.1f"), DisplayValue);
			break;

		case EGameplayModOp::Multiplicitive:
			// 배율: 1.2 → +20%
			ValueString = FString::Printf(TEXT("%.0f%%"), (DisplayValue - 1.f) * 100.f);
			break;

		case EGameplayModOp::Division:
			// 나눗셈: 0.5 → 2배 증가 → +100%
			ValueString = FString::Printf(TEXT("%.0f%%"), (1.f / DisplayValue - 1.f) * 100.f);
			break;

		case EGameplayModOp::Override:
			// 단순 값 표시
			ValueString = FString::Printf(TEXT("%.1f"), DisplayValue);
			break;

		default:
			ValueString = FString::Printf(TEXT("%.1f"), DisplayValue);
			break;
		}

		ValueText->SetText(FText::FromString(ValueString));
	}

	// ----------------------------
	// 4) 스택 텍스트
	// ----------------------------
	if (StackText)
	{
		if (StackCount > 1)
		{
			StackText->SetText(FText::AsNumber(StackCount));
			StackText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			StackText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// ----------------------------
	// 5) 남은 시간 ProgressBar
	// Duration = 남은 시간
	// MaxDuration = Data에 넣거나 외부에서 따로 관리?
	// 일단 0~1 수동으로 업데이트하도록 초기화
	// ----------------------------
	TotalDuration = Duration;
	RemainingDuration = Duration;
	
	if (DurationProgressBar)
	{
		DurationProgressBar->SetPercent(0.f);
	}

	// ----------------------------
	// 6) 남은 시간 텍스트
	// ----------------------------
	if (DurationText)
	{
		FString TimeString = FString::Printf(TEXT("%.1fs"), Duration);
		DurationText->SetText(FText::FromString(TimeString));
	}

	StartDurationTimer(Duration);

	bIsBuff = bSettingIsBuff;
}

void UBuffDebuffIconWidget::StartDurationTimer(float Duration)
{
	if (Duration <= 0.f) return;

	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().SetTimer(
		DurationTimerHandle,
		this,
		&UBuffDebuffIconWidget::UpdateDuration,
		0.05f, // 20fps 정도로 부드럽게
		true
	);
}

void UBuffDebuffIconWidget::UpdateDuration()
{
	RemainingDuration -= 0.05f;

	// 0 이하 → 버프 종료
	if (RemainingDuration <= 0.f)
	{
		RemainingDuration = 0.f;

		// ProgressBar 100%
		if (DurationProgressBar)
			DurationProgressBar->SetPercent(1.f);

		// 시간 텍스트 0.0s
		if (DurationText)
			DurationText->SetText(FText::FromString(TEXT("0.0s")));

		// 타이머 종료
		GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
		return;
	}

	// 진행률 계산
	float Ratio = 1.f - (RemainingDuration / TotalDuration);

	if (DurationProgressBar)
		DurationProgressBar->SetPercent(Ratio);

	if (DurationText)
	{
		DurationText->SetText(
			FText::FromString(FString::Printf(TEXT("%.1fs"), RemainingDuration))
		);
	}
}

void UBuffDebuffIconWidget::SetDuration(float NewDuration)
{
	RemainingDuration = NewDuration;
	TotalDuration = NewDuration;

	if (!GetWorld()->GetTimerManager().IsTimerActive(DurationTimerHandle) && RemainingDuration > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(DurationTimerHandle, this, 
			&UBuffDebuffIconWidget::UpdateDuration, 0.1f, true);
	}
}

void UBuffDebuffIconWidget::SetStackCount(int32 NewStackCount)
{
	if (StackText)
	{
		if (NewStackCount > 1)
		{
			StackText->SetText(FText::AsNumber(NewStackCount));
			StackText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			StackText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
