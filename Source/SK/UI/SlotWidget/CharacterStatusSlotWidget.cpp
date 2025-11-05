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
		AttributeSet->OnStaminaChanged.RemoveAll(this);
		AttributeSet = nullptr;
	}
	
	AttributeSet = Cast<USKAttributeSet>(ASC->GetAttributeSet(USKAttributeSet::StaticClass()));
	
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HealthChanged] AttributeSet NO"));
		return;
	}
	
	AttributeSet->OnHealthChanged.AddUObject(this, &UCharacterStatusSlotWidget::HealthChanged);
	AttributeSet->OnStaminaChanged.AddUObject(this, &UCharacterStatusSlotWidget::StaminaChanged);
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
	
	const float Percent = NewValue / FMath::Max(AttributeSet->GetMaxHealth(), 1.0f);

	HealthProgressBar->SetPercent(Percent);
}

void UCharacterStatusSlotWidget::StaminaChanged(AActor* EffectInstigator, AActor* EffectCauser,
	const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
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
	UE_LOG(LogTemp, Log, TEXT("[StaminaChanged] Old: %.2f, New: %.2f, Delta: %.2f, Percent: %.2f"), 
		OldValue, 
		NewValue, 
		EffectMagnitude, 
		NewValue / FMath::Max(AttributeSet->GetMaxHealth(), 1.0f));
	
	const float Percent = NewValue / FMath::Max(AttributeSet->GetMaxStamina(), 1.0f);

	StaminaProgressBar->SetPercent(Percent);
}
