// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Monster/MonsterDamageWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "GameAbilitySystem/Attribute/AI/SKAIAttributeSet.h"


void UMonsterDamageWidget::SettingWidget(APawn* OwnerPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("PossessPawnChanged O"));	
	if (!OwnerPawn)
	{
		return;
	}

	UAbilitySystemComponent* ASC = OwnerPawn->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC)
	{
		return;
	}
		
	if (AttributeSet)
	{
		AttributeSet->OnCurrentHealthChanged.RemoveAll(this);
		AttributeSet = nullptr;
	}
	
	AttributeSet = Cast<USKAIAttributeSet>(ASC->GetAttributeSet(USKAIAttributeSet::StaticClass()));
	
	if (!AttributeSet)
	{
		return;
	}
	
	AttributeSet->OnCurrentHealthChanged.AddUObject(this, &UMonsterDamageWidget::HealthChanged);
}

void UMonsterDamageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);
}

void UMonsterDamageWidget::HealthChanged(AActor* EffectInstigator, AActor* EffectCauser,
	const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	if (!DamageText || !AttributeSet)
	{
		return;
	}
	const float DamageValue = OldValue - NewValue;
	
	if (DamageValue <= 0.f)
	{
		return;
	}

	FText DamageTextValue = FText::AsNumber(FMath::RoundToInt(DamageValue));
	DamageText->SetText(DamageTextValue);
	
	SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().ClearTimer(HideTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &UMonsterDamageWidget::HideDamageText, 1.0f, false);
	
	FVector2D RandomOffset = FVector2D(
		FMath::FRandRange(-10.f, 10.f),
		FMath::FRandRange(-40.f, -20.f)
	);
	
	DamageText->SetRenderTranslation(RandomOffset);
}

void UMonsterDamageWidget::HideDamageText()
{
	if (DamageText)
	{
		DamageText->SetRenderTranslation(FVector2D::ZeroVector);
	}

	SetVisibility(ESlateVisibility::Hidden);
}