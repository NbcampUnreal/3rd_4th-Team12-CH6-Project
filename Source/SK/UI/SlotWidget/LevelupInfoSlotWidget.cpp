// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/LevelupInfoSlotWidget.h"

#include "Components/Overlay.h"
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

void ULevelupInfoSlotWidget::OnLevelUpResultMessageReceived(FGameplayTag Channel,
	const FPlayerLevelUpResultMessage& Message)
{
	if (Message.bResult)
	{
		int32 PlayerLevel = CurrentPS->GetPlayerLevel();
		SettingLevelUpDataByLevel(PlayerLevel);
	}
	else
	{
		LevelUpFailPlayAnim();
	}
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

	LevelUpResultHandle = MessageSubsystem->RegisterListener<FPlayerLevelUpResultMessage>(
		TAG_Message_Channel_PlayerLevelUpResult,
		this,
		&ULevelupInfoSlotWidget::OnLevelUpResultMessageReceived
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
	
	if (LevelUpResultHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(LevelUpResultHandle);
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
    
	const FLevelUpData* GoldLevelData = SDS->GetData<FLevelUpData>(Level);
	if (!GoldLevelData)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetLevelUpDataByLevel: LevelData is nullptr for Level %d"), Level);
		return;
	}

	if (CurrentLevelText)
	{
		CurrentLevelText->SetText(FText::AsNumber(Level));
	}

	if (NextLevelText)
	{
		NextLevelText->SetText(FText::AsNumber(Level+1));
	}

	const int32 CurrentGold = CurrentPS->GetGold();
	const int32 RequiredGold = GoldLevelData->RequiredGold;
	
	if (RequireGold)
	{
		RequireGold->SetText(FText::AsNumber(RequiredGold));
	}

	if (PlayerGold)
	{
		PlayerGold->SetText(FText::AsNumber(CurrentGold));
	}

	const int32 LackGold = CurrentGold - RequiredGold;

	if (ResultGold)
	{
		ResultGold->SetText(FText::AsNumber(LackGold));
		if (LackGold > 0)
		{
			ResultGold->SetColorAndOpacity(FLinearColor::Green);
		}
		else if (LackGold < 0)
		{
			ResultGold->SetColorAndOpacity(FLinearColor::Red);
		}
	}

	const FLevelUpData* StatLevelData = SDS->GetData<FLevelUpData>(Level+1);
	if (!StatLevelData)
	{
		if (UpStatMaxHealth)
		{
			UpStatMaxHealth->SetText(FText());
		}
		if (UpStatAttack)
		{
			UpStatAttack->SetText(FText());
		}
		if (UpStatArmor)
		{
			UpStatArmor->SetText(FText());
		}
		return;
	}
	
	if (UpStatMaxHealth)
	{
		UpStatMaxHealth->SetText(FText::AsNumber(StatLevelData->MaxHP));
	}

	if (UpStatAttack)
	{
		UpStatAttack->SetText(FText::AsNumber(StatLevelData->Attack));
	}

	if (UpStatArmor)
	{
		UpStatArmor->SetText(FText::AsNumber(StatLevelData->Armor));
	}
}

void ULevelupInfoSlotWidget::LevelUpFailPlayAnim()
{
	if (!LevelUpFailAnim)
	{
		return;
	}

	// 재생 중이면 중지
	if (IsAnimationPlaying(LevelUpFailAnim))
	{
		StopAnimation(LevelUpFailAnim);
	}

	// 처음부터 다시 재생
	PlayAnimation(LevelUpFailAnim, 0.f, 1);
}
