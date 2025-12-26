// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Monster/MonsterDamageContainerWidget.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "MonsterDamageWidget.h"
#include "Character/AI/SKAICharacterBase.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "GameAbilitySystem/Attribute/AI/SKAIAttributeSet.h"

void UMonsterDamageContainerWidget::SettingWidget(APawn* OwnerPawn)
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
	
	ASC->GetGameplayAttributeValueChangeDelegate(USKAIAttributeSet::GetHealthAttribute()).AddUObject(this, &UMonsterDamageContainerWidget::AddDamageText);
}

void UMonsterDamageContainerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	for (int32 i = 0; i < PoolSize; ++i)
	{
		UMonsterDamageWidget* Entry =
			CreateWidget<UMonsterDamageWidget>(GetWorld(), DamageTextEntryClass);

		UCanvasPanelSlot* CanvasSlot =
			DamageCanvas->AddChildToCanvas(Entry);

		CanvasSlot ->SetAutoSize(true);

		Entry->SetVisibility(ESlateVisibility::Collapsed);
		DamageTextPool.Add(Entry);
	}
}

void UMonsterDamageContainerWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UMonsterDamageContainerWidget::AddDamageText(const FOnAttributeChangeData& Data)
{
	const float Damage = Data.OldValue - Data.NewValue;

	if (Damage <= 0.0f)
	{
		return;
	}
	
	int32 AttackType = -1;

	if (const FGameplayEffectModCallbackData* ModData = Data.GEModData)
	{
		const FGameplayEffectSpec& EffectSpec = ModData->EffectSpec;

		if (EffectSpec.DynamicGrantedTags.HasTag(
			FGameplayTag::RequestGameplayTag(TEXT("Attack.Normal"))))
		{
			AttackType = 0;
		}
		else if (EffectSpec.DynamicGrantedTags.HasTag(
			FGameplayTag::RequestGameplayTag(TEXT("Attack.Heavy"))))
		{
			AttackType = 1;
		}
		else if (EffectSpec.DynamicGrantedTags.HasTag(
			FGameplayTag::RequestGameplayTag(TEXT("Attack.UnGuardable"))))
		{
			AttackType = 2;
		}
	}


	UMonsterDamageWidget* Entry = nullptr;

	for (int32 i = 0; i < DamageTextPool.Num(); ++i)
	{
		if (!DamageTextPool[i]->IsActive())
		{
			Entry = DamageTextPool[i];
			break;
		}
	}

	if (!Entry)
	{
		Entry = CreateWidget<UMonsterDamageWidget>(GetWorld(), DamageTextEntryClass);
		UCanvasPanelSlot* CanvasSlot = DamageCanvas->AddChildToCanvas(Entry);
		CanvasSlot->SetAutoSize(true);
		DamageTextPool.Add(Entry);
	}

	const FVector2D RandPos(
		FMath::RandRange(0.f, 300.f),
		FMath::RandRange(-60.f, -10.f)
	);
	
	Entry->SettingWidget(Damage, RandPos, AttackType);
}
