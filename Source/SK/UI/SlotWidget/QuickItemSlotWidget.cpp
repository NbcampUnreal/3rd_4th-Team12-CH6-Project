// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/QuickItemSlotWidget.h"

#include "Component/InventoryComponent.h"
#include "Component/QuickSlotComponent.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Item/Inventory/Data/SKInventoryItemData.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKNativeGameplayTags.h"

void UQuickItemSlotWidget::NativeConstruct()
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
	LayoutSwitchHandle = MessageSubsystem->RegisterListener<FSwitchLayoutMessage>(
		TAG_Message_Channel_SwitchLayout,
		this,
		&UQuickItemSlotWidget::OnSwitchLayoutMessageReceived
	);

	QuickSlotItemUseHandle = MessageSubsystem->RegisterListener<FQuickSlotCooldown>(
		TAG_Message_Channel_UseQuickSlotItem,
		this,
		&UQuickItemSlotWidget::OnQuickSlotItemUseMessageReceived
	);
	
	DefaultBrush = ItemImage1->GetBrush();
	
	TryCachedComponent();
}

void UQuickItemSlotWidget::NativeDestruct()
{
	if (CachedQuickSlot)
	{
		CachedQuickSlot->OnQuickSlotsUpdated.RemoveDynamic(this, &UQuickItemSlotWidget::SettingWidgetIcons);
	}

	if (LayoutSwitchHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(LayoutSwitchHandle);
		}
	}

	if (QuickSlotItemUseHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(QuickSlotItemUseHandle);
		}
	}
	
	Super::NativeDestruct();
}

void UQuickItemSlotWidget::TryCachedComponent()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ASKPlayerState* PS = PC->GetPlayerState<ASKPlayerState>();
	if (!PS)
	{
		// 다음 틱에 다시 시도
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryCachedComponent(); });
		return;
	}

	CachedQuickSlot = PS->FindComponentByClass<UQuickSlotComponent>();
	
	if (!CachedQuickSlot)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryCachedComponent(); });
		return;
	}

	CachedInventory = PS->FindComponentByClass<UInventoryComponent>();
	
	if (!CachedInventory)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryCachedComponent(); });
		return;
	}

	CachedQuickSlot->OnQuickSlotsUpdated.RemoveDynamic(this, &UQuickItemSlotWidget::SettingWidgetIcons);
	CachedQuickSlot->OnQuickSlotsUpdated.AddDynamic(this, &UQuickItemSlotWidget::SettingWidgetIcons);
	
	SettingWidgetIcons();
}

void UQuickItemSlotWidget::SettingWidgetIcons()
{
	if (!CachedQuickSlot)
		return;

	TArray<FQuickSlot> QuickSlots = CachedQuickSlot->GetQuickSlots();
	if (ItemImage1)
	{
		if (QuickSlots[0].ItemID == -1)
		{
			ItemImage1->SetBrush(DefaultBrush);
		}
		else
		{
			USKInventoryItemData* TempData = CachedInventory->GetItemDataByID(QuickSlots[0].ItemID);
			ItemImage1->SetBrushFromTexture(TempData->ItemIcon);			
		}
	}

	if (CountText1)
	{
		if (QuickSlots[0].ItemID == -1)
		{
			CountText1->SetText(FText::GetEmpty());
		}
		else
		{
			CountText1->SetText(FText::AsNumber(QuickSlots[0].Count));
		}
	}

	if (ItemImage2)
	{
		if (QuickSlots[1].ItemID == -1)
		{
			ItemImage2->SetBrush(DefaultBrush);
		}
		else
		{
			USKInventoryItemData* TempData = CachedInventory->GetItemDataByID(QuickSlots[1].ItemID);
			ItemImage2->SetBrushFromTexture(TempData->ItemIcon);			
		}
	}

	if (CountText2)
	{
		if (QuickSlots[1].ItemID == -1)
		{
			CountText2->SetText(FText::GetEmpty());
		}
		else
		{
			CountText2->SetText(FText::AsNumber(QuickSlots[1].Count));
		}
	}

	if (ItemImage3)
	{
		if (QuickSlots[2].ItemID == -1)
		{
			ItemImage3->SetBrush(DefaultBrush);
		}
		else
		{
			USKInventoryItemData* TempData = CachedInventory->GetItemDataByID(QuickSlots[2].ItemID);
			ItemImage3->SetBrushFromTexture(TempData->ItemIcon);			
		}
	}

	if (CountText3)
	{
		if (QuickSlots[2].ItemID == -1)
		{
			CountText3->SetText(FText::GetEmpty());
		}
		else
		{
			CountText3->SetText(FText::AsNumber(QuickSlots[2].Count));
		}
	}
	
}

void UQuickItemSlotWidget::StartCooldown(int32 SlotIndex, float Duration)
{
	CooldownDurations[SlotIndex] = Duration;
	CooldownElapsed[SlotIndex] = 0.f;
 
	UProgressBar* TargetBar = nullptr;
	
	switch (SlotIndex)
	{
	case 0: TargetBar = CooldownProgressBar1; break;
	case 1: TargetBar = CooldownProgressBar2; break;
	case 2: TargetBar = CooldownProgressBar3; break;
	default: break;
	}
 
	if (TargetBar)
	{
		TargetBar->SetPercent(1.0f);
	}
 
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerHandles[SlotIndex],
		FTimerDelegate::CreateUObject(this, &UQuickItemSlotWidget::UpdateCooldownProgress, SlotIndex),
		0.02f,
		true
	);
}

void UQuickItemSlotWidget::UpdateCooldownProgress(int32 SlotIndex)
{
	CooldownElapsed[SlotIndex] += 0.02f;
	float Remaining = FMath::Clamp(CooldownDurations[SlotIndex] - CooldownElapsed[SlotIndex], 0.f, CooldownDurations[SlotIndex]);
	float Percent = Remaining / CooldownDurations[SlotIndex];
 
	UProgressBar* TargetBar = nullptr;
	switch (SlotIndex)
	{
	case 0: TargetBar = CooldownProgressBar1; break;
	case 1: TargetBar = CooldownProgressBar2; break;
	case 2: TargetBar = CooldownProgressBar3; break;
	default: break;
	}
 
	if (TargetBar)
	{
		TargetBar->SetPercent(Percent);
	}
 
	if (Remaining <= 0.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandles[SlotIndex]);
		if (TargetBar)
		{
			TargetBar->SetPercent(0.f);
		}
	}
}

void UQuickItemSlotWidget::OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message)
{
	if (Message.LayoutTag != TAG_UI_Layout_InGame)
	{
		return;
	}
	SettingWidgetIcons();
}

void UQuickItemSlotWidget::OnQuickSlotItemUseMessageReceived(FGameplayTag Channel, const FQuickSlotCooldown& Message)
{
	// 로그 출력
	UE_LOG(LogTemp, Log, TEXT("[QuickSlot] SlotIndex: %d, Cooldown: %.2f"), Message.SlotIndex, Message.Cooldown);
	if (Message.SlotIndex >= 0 && Message.SlotIndex < 3)
	{
		StartCooldown(Message.SlotIndex, Message.Cooldown);
	}
}
