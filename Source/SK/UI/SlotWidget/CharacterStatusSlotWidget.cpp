// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/CharacterStatusSlotWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "PlayerState/SKPlayerState.h"

void UCharacterStatusSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
 
	TryBind();
}

void UCharacterStatusSlotWidget::NativeDestruct()
{
	if (AttributeSet)
	{
		AttributeSet->OnHealthChanged.RemoveAll(this);
		AttributeSet->OnStaminaChanged.RemoveAll(this);
		AttributeSet->OnHeatChanged.RemoveAll(this);
		AttributeSet->OnMaxHeatChanged.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UCharacterStatusSlotWidget::TryBind()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) 
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryBind(); });
		return;
	}
 
	APlayerState* PS = PC->GetPlayerState<APlayerState>();
	if (!PS) 
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryBind(); });
		return;
	}

	ASKPlayerState* CurrentPS = Cast<ASKPlayerState>(PS);
	if (!CurrentPS) 
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryBind(); });
		return;
	}
	
	UAbilitySystemComponent* ASC = CurrentPS->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryBind(); });
		return;
	}
 
	// 기존 바인딩 해제
	if (AttributeSet)
	{
		AttributeSet->OnHealthChanged.RemoveAll(this);
		AttributeSet->OnStaminaChanged.RemoveAll(this);
		AttributeSet->OnHeatChanged.RemoveAll(this);
		AttributeSet->OnMaxHeatChanged.RemoveAll(this);
		AttributeSet = nullptr;
	}
 
	AttributeSet = Cast<USKAttributeSet>(ASC->GetAttributeSet(USKAttributeSet::StaticClass()));
	if (!AttributeSet) 
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryBind(); });
		return;
	}
 
	AttributeSet->OnHealthChanged.AddUObject(this, &UCharacterStatusSlotWidget::HealthChanged);
	AttributeSet->OnStaminaChanged.AddUObject(this, &UCharacterStatusSlotWidget::StaminaChanged);
	AttributeSet->OnHeatChanged.AddUObject(this, &UCharacterStatusSlotWidget::HeatChanged);
	AttributeSet->OnMaxHeatChanged.AddUObject(this, &UCharacterStatusSlotWidget::MaxHeatChanged);
 
 
	// 초기 UI 업데이트
	HealthChanged(nullptr, nullptr, nullptr, 0.f, 0.f, AttributeSet->GetHealth());
	StaminaChanged(nullptr, nullptr, nullptr, 0.f, 0.f, AttributeSet->GetStamina());
	MaxHeatChanged(nullptr, nullptr, nullptr, 0.f, 0.f, AttributeSet->GetMaxHeat());
	HeatChanged(nullptr, nullptr, nullptr, 0.f, 0.f, AttributeSet->GetHeat());
}

void UCharacterStatusSlotWidget::HealthChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue) const
{
	if (!HealthProgressBar)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HealthChanged] HealthProgressBar NO"));
		return;
	}
	
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HealthChanged] AttributeSet NO"));
		return;
	}
	
	const float Percent = NewValue / FMath::Max(AttributeSet->GetMaxHealth(), 1.0f);

	HealthProgressBar->SetPercent(Percent);
}

void UCharacterStatusSlotWidget::StaminaChanged(AActor* EffectInstigator, AActor* EffectCauser,
	const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue) const
{
	if (!StaminaProgressBar)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StaminaChanged] StaminaProgressBar NO"));
		return;
	}
	
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StaminaChanged] AttributeSet NO"));
		return;
	}
	
	const float Percent = NewValue / FMath::Max(AttributeSet->GetMaxStamina(), 1.0f);

	StaminaProgressBar->SetPercent(Percent);
}

void UCharacterStatusSlotWidget::HeatChanged(AActor* EffectInstigator, AActor* EffectCauser,
	const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue) const
{
	for (int32 i = 0; i < HeatBars.Num(); ++i)
	{
		const float BarFill = FMath::Clamp(NewValue - i, 0.f, 1.f);
		HeatBars[i]->SetPercent(BarFill);
	}
}

void UCharacterStatusSlotWidget::MaxHeatChanged(AActor* EffectInstigator, AActor* EffectCauser,
	const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	if (!HeatContainer) return;

	HeatContainer->ClearChildren();
	HeatBars.Empty();

	for (int32 i = 0; i < AttributeSet->GetMaxHeat(); ++i)
	{
		UProgressBar* NewHeatBar = NewObject<UProgressBar>(this, UProgressBar::StaticClass());
		HeatContainer->AddChild(NewHeatBar);

		// 기본 설정
		NewHeatBar->SetPercent(0.f);

		// 아이콘처럼 보이게 스타일 세팅
		FProgressBarStyle Style;
		Style.BackgroundImage.SetResourceObject(EmptyHeatTexture);
		Style.FillImage.SetResourceObject(FullHeatTexture);
		Style.FillImage.DrawAs = ESlateBrushDrawType::Image;
		Style.BackgroundImage.DrawAs = ESlateBrushDrawType::Image;

		NewHeatBar->SetWidgetStyle(Style);

		// HorizontalBoxSlot 패딩
		if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(NewHeatBar->Slot))
		{
			HBoxSlot->SetPadding(FMargin(10.f, 0.f, 10.f, 0.f));
		}

		HeatBars.Add(NewHeatBar);
	}
}
