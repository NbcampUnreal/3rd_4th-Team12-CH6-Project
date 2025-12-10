// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/Equipment/CharacterStatSlotWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/TextBlock.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UCharacterStatSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	TryBind();

	UWorld* World = GetWorld();
	if (!World)
		return;
		
	UGameInstance* GameInstance = World->GetGameInstance();
	if(!GameInstance)
		return;
		
	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
		return;

	// 메시지 구독
	LayoutSwitchHandle = MessageSubsystem->RegisterListener<FSwitchLayoutMessage>(
		TAG_Message_Channel_SwitchLayout,
		this,
		&UCharacterStatSlotWidget::OnSwitchLayoutMessageReceived
	);
}

void UCharacterStatSlotWidget::NativeDestruct()
{
	if (LayoutSwitchHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(LayoutSwitchHandle);
		}
	}
	Super::NativeDestruct();
}

void UCharacterStatSlotWidget::TryBind()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		RetryLater(TEXT("No PlayerController"));
		return;
	}

	APlayerState* PS = PC->GetPlayerState<APlayerState>();
	if (!PS)
	{
		RetryLater(TEXT("No PlayerState"));
		return;
	}

	UAbilitySystemComponent* ASC = PS->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC)
	{
		RetryLater(TEXT("No ASC on PlayerState"));
		return;
	}

	CharacterStat = ASC->GetSet<USKAttributeSet>();
	if (!CharacterStat)
	{
		RetryLater(TEXT("No USKAttributeSet"));
		return;
	}

	// 성공
	UE_LOG(LogTemp, Log, TEXT("[CharacterStatSlot] Bind SUCCESS"));
	ReflashStat();
}

void UCharacterStatSlotWidget::RetryLater(const TCHAR* Reason)
{
	RetryCount++;

	UE_LOG(LogTemp, Warning, TEXT("[CharacterStatSlot] Bind failed (%s). Retry %d/%d"),
		Reason, RetryCount, MaxRetryCount);

	if (RetryCount >= MaxRetryCount)
	{
		UE_LOG(LogTemp, Error, TEXT("[CharacterStatSlot] Bind FAILED permanently."));
		return;
	}

	// 0.2초 후 재시도
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			RetryBindTimer,
			FTimerDelegate::CreateUObject(this, &UCharacterStatSlotWidget::TryBind),
			0.2f,
			false
		);
	}
}

void UCharacterStatSlotWidget::OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message)
{
	if (Message.LayoutTag != TAG_UI_Layout_EquipmentMain)
	{
		UE_LOG(LogTemp, Log, TEXT("UInventoryListSlotWidget No Layout Inventory"));
		return;
	}
	ReflashStat();
}

void UCharacterStatSlotWidget::ReflashStat()
{
	if (!CharacterStat)
	{
		UE_LOG(LogTemp, Warning, TEXT("ReflashStat Failed: CharacterStat is null"));
		return;
	}
	SetStatText(MaxHealthText, CharacterStat->GetMaxHealth());
	SetStatText(MaxStaminaText, CharacterStat->GetMaxStamina());
	SetStatText(AttackPowerText, CharacterStat->GetAttack());
	SetStatText(DefensePowerText, CharacterStat->GetArmor());
	SetStatText(LevelText, CharacterStat->GetLevel());
}

void UCharacterStatSlotWidget::SetStatText(UTextBlock* Text, float Value)
{
	if (Text)
	{
		Text->SetText(FText::AsNumber(FMath::FloorToInt(Value)));
	}
}
