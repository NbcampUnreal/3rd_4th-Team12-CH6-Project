// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/Dungeon/DungeonStartSlotWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UDungeonStartSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
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
	DungeonStartHandle = MessageSubsystem->RegisterListener<FLoadingUIVisible>(
		TAG_Message_Channel_LoadingUIVisible,
		this,
		&UDungeonStartSlotWidget::OnDungeonStartMessageReceived
	);
}

void UDungeonStartSlotWidget::PlayAppearSound()
{
	if (AppearSound)
	{
		UGameplayStatics::PlaySound2D(this, AppearSound);
		if (MapText)
		{
			const FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true); 
			MapText->SetText(FText::FromString(LevelName));
		}
	}
}

void UDungeonStartSlotWidget::NativeDestruct()
{
	if (DungeonStartHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(DungeonStartHandle);
		}
	}
	Super::NativeDestruct();
}

void UDungeonStartSlotWidget::PlayAppearAnimation()
{
	if (AppearAnim)
	{
		PlayAnimation(AppearAnim);
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&UDungeonStartSlotWidget::PlayAppearSound,
			0.5f,
			false
		);
	}
}

void UDungeonStartSlotWidget::OnDungeonStartMessageReceived(FGameplayTag Channel, const FLoadingUIVisible& Message)
{
	if (!Message.bVisible)
	{
		if (MapText)
		{
			const FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true); 
			MapText->SetText(FText::FromString(LevelName));
		}
		PlayAppearAnimation();
	}
}
