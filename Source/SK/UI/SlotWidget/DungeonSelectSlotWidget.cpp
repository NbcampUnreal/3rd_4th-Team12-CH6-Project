// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/DungeonSelectSlotWidget.h"

#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "GameInstance/SKGameInstance.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UDungeonSelectSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Dungeon1Button)
	{
		Dungeon1Button->OnHovered.AddDynamic(this, &UDungeonSelectSlotWidget::OnDungeon1Hovered);
		Dungeon1Button->OnUnhovered.AddDynamic(this, &UDungeonSelectSlotWidget::OnDungeon1Unhovered);
		Dungeon1Button->OnClicked.AddDynamic(this, &UDungeonSelectSlotWidget::OnDungeon1Clicked);
	}
 
	// Bind hover events for Dungeon 2 button
	if (Dungeon2Button)
	{
		Dungeon2Button->OnHovered.AddDynamic(this, &UDungeonSelectSlotWidget::OnDungeon2Hovered);
		Dungeon2Button->OnUnhovered.AddDynamic(this, &UDungeonSelectSlotWidget::OnDungeon2Unhovered);
		Dungeon2Button->OnClicked.AddDynamic(this, &UDungeonSelectSlotWidget::OnDungeon2Clicked);
	}
 
	// Bind close button click event
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UDungeonSelectSlotWidget::OnCloseClicked);
	}
 
	// Initially hide both dungeon info boxes
	if (Dungeon1InfoBox)
	{
		Dungeon1InfoBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (Dungeon2InfoBox)
	{
		Dungeon2InfoBox->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDungeonSelectSlotWidget::OnDungeon1Hovered()
{
	SetDungeonInfoVisibility(Dungeon1InfoBox, true);
}

void UDungeonSelectSlotWidget::OnDungeon1Unhovered()
{
	SetDungeonInfoVisibility(Dungeon1InfoBox, false);
}

void UDungeonSelectSlotWidget::OnDungeon2Hovered()
{
	SetDungeonInfoVisibility(Dungeon2InfoBox, true);
}

void UDungeonSelectSlotWidget::OnDungeon2Unhovered()
{
	SetDungeonInfoVisibility(Dungeon2InfoBox, false);
}

void UDungeonSelectSlotWidget::OnDungeon1Clicked()
{
	UE_LOG(LogTemp, Log, TEXT("Dungeon 1 Selected"));
 
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null in OnDungeon1Clicked"));
		return;
	}
 
	USKGameInstance* SKGameInstance = Cast<USKGameInstance>(World->GetGameInstance());
	if (!SKGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("SKGameInstance is null in OnDungeon1Clicked"));
		return;
	}
 
	SKGameInstance->TravelToDungeon(1);
}

void UDungeonSelectSlotWidget::OnDungeon2Clicked()
{
	UE_LOG(LogTemp, Log, TEXT("Dungeon 2 Selected"));

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null in OnDungeon1Clicked"));
		return;
	}
 
	USKGameInstance* SKGameInstance = Cast<USKGameInstance>(World->GetGameInstance());
	if (!SKGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("SKGameInstance is null in OnDungeon1Clicked"));
		return;
	}
 
	SKGameInstance->TravelToDungeon(2);
}

void UDungeonSelectSlotWidget::OnCloseClicked()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FSwitchLayoutMessage Message(TAG_UI_Layout_InGame, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout, Message);

			UE_LOG(LogTemp, Log, TEXT("Broadcast SwitchLayout Message: %s"), *Message.LayoutTag.ToString());
		}
	}
}

void UDungeonSelectSlotWidget::SetDungeonInfoVisibility(USizeBox* InfoBox, bool bVisible)
{
	if (InfoBox)
	{
		InfoBox->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
