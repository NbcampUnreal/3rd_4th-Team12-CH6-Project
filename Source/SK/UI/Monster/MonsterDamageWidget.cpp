// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Monster/MonsterDamageWidget.h"

#include "Components/TextBlock.h"
#include "TimerManager.h"


void UMonsterDamageWidget::SettingWidget(float Damage, const FVector2D RendPos, int32 AttackType)
{
	bIsActive = true;
	SetVisibility(ESlateVisibility::Visible);

	if (!DamageText)
	{
		return;
	}

	DamageText->SetText(
		FText::AsNumber(FMath::RoundToInt(Damage))
	);
	
	DamageText->SetColorAndOpacity(
		GetDamageColor(AttackType)
	);
	SetRenderTranslation(RendPos);

	if (Anim_TextMove)
	{
		PlayAnimationForward(Anim_TextMove);
	}

	if (!GetWorld())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		HideTimerHandle,
		this,
		&UMonsterDamageWidget::Deactivate,
		2.0f,
		false
	);
}

void UMonsterDamageWidget::Deactivate()
{
	bIsActive = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

void UMonsterDamageWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FSlateColor UMonsterDamageWidget::GetDamageColor(int32 AttackType)
{
	switch (AttackType)
	{
	case 0: // Normal
		return FSlateColor(FLinearColor::Black);
	case 1: // Heavy
		return FSlateColor(FLinearColor(0.4f, 0.0f, 0.0f));
	case 2: // Unguardable
		return FSlateColor(FLinearColor(0.5f, 0.f, 0.8f)); // 보라
	default:
		return FSlateColor(FLinearColor::Black);
	}
}
