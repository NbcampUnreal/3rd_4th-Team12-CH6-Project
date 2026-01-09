// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Monster/BossHPSlotWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameAbilitySystem/Attribute/AI/SKAIAttributeSet.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UBossHPSlotWidget::SettingWidget(APawn* OwnerPawn)
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
		AttributeSet->OnCurrentPoiseChanged.RemoveAll(this);
		AttributeSet = nullptr;
	}
	
	AttributeSet = Cast<USKAIAttributeSet>(ASC->GetAttributeSet(USKAIAttributeSet::StaticClass()));
	
	if (!AttributeSet)
	{
		return;
	}
	
	AttributeSet->OnCurrentHealthChanged.AddUObject(this, &UBossHPSlotWidget::HealthChanged);
	AttributeSet->OnCurrentPoiseChanged.AddUObject(this, &UBossHPSlotWidget::PoiseChanged);

	HealthChanged(nullptr, nullptr, nullptr, 0, AttributeSet->GetHealth(), AttributeSet->GetHealth());
}

void UBossHPSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("[BossWidget] NativeConstruct 호출됨"));

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[BossWidget] World 없음"));
		return;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if(!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[BossWidget] GameInstance 없음"));
		return;
	}

	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("[BossWidget] MessageSubsystem 없음"));
		return;
	}

	// 메시지 구독
	SettingBossHandle = MessageSubsystem->RegisterListener<FSettingBossHPWidget>(
		TAG_Message_Channel_BossWidgetSetting,
		this,
		&UBossHPSlotWidget::OnSettingBossMessageReceived
	);

	UE_LOG(LogTemp, Warning, TEXT("[BossWidget] 메시지 채널 구독 완료"));
}

void UBossHPSlotWidget::NativeDestruct()
{
	if (AttributeSet)
	{
		AttributeSet->OnCurrentHealthChanged.RemoveAll(this);
		AttributeSet->OnCurrentPoiseChanged.RemoveAll(this);
	}

	if (SettingBossHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(SettingBossHandle);
		}
	}
	
	Super::NativeDestruct();
}

void UBossHPSlotWidget::HealthChanged(AActor* EffectInstigator, AActor* EffectCauser,
	const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	if (!HealthProgressBar || !AttributeSet)
	{
		return;
	}

	const float MaxHealth = FMath::Max(AttributeSet->GetMaxHealth(), 1.0f);
	TargetHealthPercent = NewValue / MaxHealth;
	HealthProgressBar->SetPercent(TargetHealthPercent);
	StartHealthAnimation();
}

void UBossHPSlotWidget::PoiseChanged(AActor* EffectInstigator, AActor* EffectCauser,
	const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	if (!HealthProgressBar || !AttributeSet)
	{
		return;
	}

	const float Percent = FMath::Min(AttributeSet->GetPoise(), 1.0f)/ 100.0f;

	HealthProgressBar->SetPercent(Percent);
}

void UBossHPSlotWidget::OnSettingBossMessageReceived(FGameplayTag Channel, const FSettingBossHPWidget& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("[BossWidget] Boss 설정 메시지 수신"));
	UE_LOG(LogTemp, Warning, TEXT("[BossWidget] 채널: %s"), *Channel.ToString());

	if (!Message.OwnerActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[BossWidget] OwnerActor 없음 → 메시지 무시"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[BossWidget] OwnerActor: %s"), *Message.OwnerActor->GetName());

	SettingWidget(Message.OwnerActor);

	if (NameText)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossWidget] NameText 갱신: %s"), *Message.NameText.ToString());
		NameText->SetText(Message.NameText);
	}

	UE_LOG(LogTemp, Warning, TEXT("[BossWidget] 메시지 처리 완료"));
}

void UBossHPSlotWidget::StartHealthAnimation()
{
	if (!GetWorld())
	{
		return;
	}

	// 이미 돌고 있으면 재시작
	GetWorld()->GetTimerManager().ClearTimer(HealthAnimTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(
		HealthAnimTimerHandle,
		this,
		&UBossHPSlotWidget::AnimateHealth,
		0.016f,   // 약 60fps
		true,
		0.5f
	);
}

void UBossHPSlotWidget::AnimateHealth()
{
	if (!TargetHealthProgressBar)
	{
		StopHealthAnimation();
		return;
	}

	DisplayHealthPercent = FMath::FInterpTo(
		DisplayHealthPercent,
		TargetHealthPercent,
		0.016f,
		HealthInterpSpeed
	);

	TargetHealthProgressBar->SetPercent(DisplayHealthPercent);

	if (FMath::IsNearlyEqual(DisplayHealthPercent, TargetHealthPercent, 0.001f))
	{
		DisplayHealthPercent = TargetHealthPercent;
		TargetHealthProgressBar->SetPercent(DisplayHealthPercent);
		StopHealthAnimation();
	}
}

void UBossHPSlotWidget::StopHealthAnimation()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HealthAnimTimerHandle);
	}
}
