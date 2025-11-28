// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/ItemAddInfoWidget.h"

#include "Components/TextBlock.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UItemAddInfoWidget::NativeConstruct()
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
	
	ItemAddHandle = MessageSubsystem->RegisterListener<FItemAddMessage>(
		TAG_Message_Channel_ItemAddInfo,
		this,
		&UItemAddInfoWidget::OnAddItemMessageReceived
	);
}

void UItemAddInfoWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UItemAddInfoWidget::OnAddItemMessageReceived(FGameplayTag Channel, const FItemAddMessage& Message)
{
	if(ItemText)
	{
		FString MsgString = FString::Printf(TEXT("ItemID: %d  x%d"), Message.ItemID, Message.ItemCount);
		ItemText->SetText(FText::FromString(MsgString));
	}

	SetVisibility(ESlateVisibility::Visible);
	
	if(PopupFadeAnimation)
	{
		PlayAnimation(PopupFadeAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			SetVisibility(ESlateVisibility::Hidden);
		}, 1.5f, false);
	}
	else
	{ 
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			SetVisibility(ESlateVisibility::Hidden);
		}, 1.0f, false);
	}
}
