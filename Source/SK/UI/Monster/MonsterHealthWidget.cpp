// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Monster/MonsterHealthWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "GameAbilitySystem/Attribute/AI/SKAIAttributeSet.h"

void UMonsterHealthWidget::SettingWidget(APawn* OwnerPawn)
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
	
	AttributeSet->OnCurrentHealthChanged.AddUObject(this, &UMonsterHealthWidget::HealthChanged);
}

void UMonsterHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);
}

void UMonsterHealthWidget::NativeDestruct()
{
	if (AttributeSet)
	{
		AttributeSet->OnCurrentHealthChanged.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UMonsterHealthWidget::HealthChanged(AActor* EffectInstigator, AActor* EffectCauser,
                                         const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	if (!HealthProgressBar || !AttributeSet)
	{
		return;
	}

	const float Percent = NewValue / FMath::Max(AttributeSet->GetMaxHealth(), 1.0f);

	HealthProgressBar->SetPercent(Percent);
	
	SetVisibility(ESlateVisibility::Visible);

	// 기존 타이머 초기화 후 다시 시작 (HP가 계속 바뀌면 다시 3초 유지)
	GetWorld()->GetTimerManager().ClearTimer(HideTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		HideTimerHandle,
		this,
		&UMonsterHealthWidget::HideHealthBar,
		3.0f,
		false
	);
}

void UMonsterHealthWidget::HideHealthBar()
{
	SetVisibility(ESlateVisibility::Hidden);
}
