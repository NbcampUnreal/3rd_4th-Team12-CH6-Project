// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/CharacterStatusSlotWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Controller/SKPlayerController.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"

void UCharacterStatusSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("[HealthChanged] NativeConstruct"));
	ASKPlayerController* PC = Cast<ASKPlayerController>(GetOwningPlayer());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HealthChanged] PC NO"));
		return;

	}

	PC->OnPawnPossessed.AddUObject(this, &UCharacterStatusSlotWidget::PossessPawnChanged);
	PossessPawnChanged(PC->GetPawn());
}

void UCharacterStatusSlotWidget::PossessPawnChanged(APawn* ChangePawn)
{
	UE_LOG(LogTemp, Warning, TEXT("PossessPawnChanged O"));	
	if (!ChangePawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HealthChanged] ChangePawn NO"));
		return;
	}

	UAbilitySystemComponent* ASC = ChangePawn->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HealthChanged] ASC NO"));
		return;
	}
		
	if (AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HealthChanged] 바인드 초기화"));
		AttributeSet->OnHealthChanged.RemoveAll(this);
		AttributeSet = nullptr;
	}
	
	AttributeSet = Cast<USKAttributeSet>(ASC->GetAttributeSet(USKAttributeSet::StaticClass()));
	
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HealthChanged] AttributeSet NO"));
		return;
	}
	
	AttributeSet->OnHealthChanged.AddUObject(this, &UCharacterStatusSlotWidget::HealthChanged);
	HealthChanged(nullptr, nullptr, nullptr, 0, 0, AttributeSet->GetHealth());
}

void UCharacterStatusSlotWidget::HealthChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
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
	UE_LOG(LogTemp, Log, TEXT("[HealthChanged] Old: %.2f, New: %.2f, Delta: %.2f, Percent: %.2f"), 
		OldValue, 
		NewValue, 
		EffectMagnitude, 
		NewValue / FMath::Max(AttributeSet->GetMaxHealth(), 1.0f));
	
	// 체력 비율 계산
	const float Percent = NewValue / FMath::Max(AttributeSet->GetMaxHealth(), 1.0f);

	// ProgressBar 업데이트
	HealthProgressBar->SetPercent(Percent);
}
