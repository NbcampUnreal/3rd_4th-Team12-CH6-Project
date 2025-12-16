// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SKLayoutWidgetBase.h"

#include "Input/CommonUIActionRouterBase.h"
#include "UI/SKSlotBox.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

USKLayoutWidgetBase::USKLayoutWidgetBase()
{

}

void USKLayoutWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Hidden);

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
	SlotVisibleHandle = MessageSubsystem->RegisterListener<FSlotVisibilityMessage>(
		TAG_Message_Channel_SlotVisible,
		this,
		&USKLayoutWidgetBase::OnSlotVisibilityMessageReceived
	);
}

void USKLayoutWidgetBase::NativeDestruct()
{
	if (SlotVisibleHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(SlotVisibleHandle);
		}
	}
	
	Super::NativeDestruct();
}


void USKLayoutWidgetBase::SetSlotData(const TArray<FSlotWidgetData>& NewSlots, FGameplayTag NewLayoutTag)
{
	SlotWidgetDataArray = NewSlots;
	LayoutTag = NewLayoutTag;
	NotifySlotDataChanged();
}

//ActivateWidget을 했을 때 바뀔 것 들을 설정해야 함
TOptional<FUIInputConfig> USKLayoutWidgetBase::GetDesiredInputConfig() const
{
	/*
	 * ECommonInputMode Input 모드 설정 Game: InGame, Menu: UI, All: 둘다
	 * 마우스 캡처 모드
	 * 마우스 룩 모드
	 * 마우스 커서 숨김 여부
	 */
	return ActivaeInputConfig;
}

TArray<FSlotWidgetData> USKLayoutWidgetBase::GetSlotData()
{
	return SlotWidgetDataArray;
}

USKSlotBox* USKLayoutWidgetBase::FindDynamicEntryBoxBySlotTag(const FGameplayTag& InSlotTag) const
{
	if (!WidgetTree)
	{
		return nullptr;
	}
 
	TArray<UWidget*> Widgets;
	WidgetTree->GetAllWidgets(Widgets);
 
	for (UWidget* Widget : Widgets)
	{
		USKSlotBox* TaggedEntryBox = Cast<USKSlotBox>(Widget);
		if (TaggedEntryBox && TaggedEntryBox->BoxSlotTag == InSlotTag)
		{
			return TaggedEntryBox;
		}
	}
	return nullptr;
}

void USKLayoutWidgetBase::NativeOnActivated()
{
	Super::NativeOnActivated();
}

void USKLayoutWidgetBase::ChangeVisibleSlotByTag(FGameplayTag ChangeSlotTag, bool bvisible)
{
	USKSlotBox* ChangeSlot = FindDynamicEntryBoxBySlotTag(ChangeSlotTag);
	if (!ChangeSlot)
	{
		return;
	}

	if (bvisible)
	{
		ChangeSlot->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ChangeSlot->SetVisibility(ESlateVisibility::Hidden);
	}
}


void USKLayoutWidgetBase::OnSlotVisibilityMessageReceived(FGameplayTag Channel, const FSlotVisibilityMessage& Message)
{
	if (Message.LayoutTag != LayoutTag)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;

	FTimerManager& TimerManager = World->GetTimerManager();

	for (const FGameplayTag& SlotTag : Message.SlotTags)
	{
		// 가시성 변경
		ChangeVisibleSlotByTag(SlotTag, Message.bVisible);
		UE_LOG(LogTemp, Log, TEXT("[Layout] SlotVisibilityMessage: %s -> %s"),
			*SlotTag.ToString(), Message.bVisible ? TEXT("Visible") : TEXT("Hidden"));

		// 기존 타이머가 있으면 초기화
		if (FTimerHandle* ExistingHandle = SlotTimerHandles.Find(SlotTag))
		{
			TimerManager.ClearTimer(*ExistingHandle);
		}

		// 새 타이머 설정 (VisibleDuration이 0보다 큰 경우)
		if (Message.VisibleDuration > 0.f)
		{
			FTimerHandle& NewHandle = SlotTimerHandles.FindOrAdd(SlotTag);
			TimerManager.SetTimer(
				NewHandle,
				FTimerDelegate::CreateLambda([this, SlotTag, bVisible = Message.bVisible]()
				{
					ChangeVisibleSlotByTag(SlotTag, !bVisible);
					UE_LOG(LogTemp, Log, TEXT("[Layout] Slot auto-hidden: %s"), *SlotTag.ToString());
				}),
				Message.VisibleDuration,
				false
			);
		}
	}
}

void USKLayoutWidgetBase::NotifySlotDataChanged() const
{
	OnSlotDataChanged.Broadcast();
}
