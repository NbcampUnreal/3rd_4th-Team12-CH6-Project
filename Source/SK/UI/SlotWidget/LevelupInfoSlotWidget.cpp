// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/LevelupInfoSlotWidget.h"

#include "Components/TextBlock.h"
#include "GameData/StaticData/LevelUpData.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Utility/StaticDataSubsystem.h"

void ULevelupInfoSlotWidget::OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message)
{
	if (Message.LayoutTag != TAG_UI_Layout_BonfireMenu)
		return;

	int32 PlayerLevel = CurrentPS->GetPlayerLevel();
	SettingLevelUpDataByLevel(PlayerLevel);
}

void ULevelupInfoSlotWidget::TryBind()
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

	CurrentPS = Cast<ASKPlayerState>(PS);
	if (!CurrentPS)
	{
		RetryLater(TEXT("No PlayerState"));
		return;
	}

	if (CurrentLevelText)
	{
		CurrentLevelText->SetText(FText::AsNumber(CurrentPS->GetPlayerLevel()));
	}

	if (NextLevelText)
	{
		NextLevelText->SetText(FText::AsNumber(CurrentPS->GetPlayerLevel()+1));
	}
}

void ULevelupInfoSlotWidget::RetryLater(const TCHAR* Reason)
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
			FTimerDelegate::CreateUObject(this, &ULevelupInfoSlotWidget::TryBind),
			0.2f,
			false
		);
	}
}

void ULevelupInfoSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TryBind();
	
	USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this);
	if (!MessageSubsystem)
		return;

	LayoutSwitchHandle = MessageSubsystem->RegisterListener<FSwitchLayoutMessage>(
		TAG_Message_Channel_SwitchLayout,
		this,
		&ULevelupInfoSlotWidget::OnSwitchLayoutMessageReceived
	);
}

void ULevelupInfoSlotWidget::NativeDestruct()
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

void ULevelupInfoSlotWidget::SettingLevelUpDataByLevel(int32 Level)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetLevelUpDataByLevel: World is nullptr"));
		return;
	}
    
	UGameInstance* GI = World->GetGameInstance();
	if (!GI)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetLevelUpDataByLevel: GameInstance is nullptr"));
		return;
	}
    
	UStaticDataSubsystem* SDS = GI->GetSubsystem<UStaticDataSubsystem>();
	if (!SDS)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetLevelUpDataByLevel: StaticDataSubsystem is nullptr"));
		return;
	}
    
	const FLevelUpData* LevelData = SDS->GetData<FLevelUpData>(Level+1);
	if (!LevelData)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetLevelUpDataByLevel: LevelData is nullptr for Level %d"), Level);
		return;
	}
    
	// 로그 출력
	UE_LOG(LogTemp, Log, TEXT("LevelUpData found for Level %d: RequiredGold=%d, MaxHP=%.1f, Attack=%.1f, Armor=%.1f, AbilityPointReward=%d"),
		Level,
		LevelData->RequiredGold,
		LevelData->MaxHP,
		LevelData->Attack,
		LevelData->Armor,
		LevelData->AbilityPointReward);

	if (CurrentLevelText)
	{
		CurrentLevelText->SetText(FText::AsNumber(Level));
	}

	if (NextLevelText)
	{
		NextLevelText->SetText(FText::AsNumber(Level+1));
	}
	
	if (RequireGold)
	{
		RequireGold->SetText(FText::AsNumber(LevelData->RequiredGold));
	}

	if (UpStatMaxHealth)
	{
		UpStatMaxHealth->SetText(FText::AsNumber(LevelData->MaxHP));
	}

	if (UpStatMaxStamina)
	{
		// 스테미나 있으면 주석 해제
		//UpStatMaxStamina->SetText(FText::AsNumber(LevelData->MaxStamina));
	}

	if (UpStatAttack)
	{
		UpStatAttack->SetText(FText::AsNumber(LevelData->Attack));
	}

	if (UpStatArmor)
	{
		UpStatArmor->SetText(FText::AsNumber(LevelData->Armor));
	}
}
