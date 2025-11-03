// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SKLayoutWidgetBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Input/CommonUIActionRouterBase.h"
#include "UI/SKSlotBox.h"

USKLayoutWidgetBase::USKLayoutWidgetBase()
{
	bIsFocusable = true;
}

void USKLayoutWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Hidden);
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("[Layout] Owning PlayerController not found!"));
		return;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Error, TEXT("[Layout] No Pawn found for PlayerController!"));
		return;
	}

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Pawn);
	if (!ASCInterface)
	{
		UE_LOG(LogTemp, Error, TEXT("[Layout] Pawn does not implement AbilitySystemInterface!"));
		return;
	}

	CachedASC = ASCInterface->GetAbilitySystemComponent();

	if (CachedASC)
	{
		UE_LOG(LogTemp, Log, TEXT("[Layout] Cached ASC O"));
		for (const FSlotEventBinding& Binding : SlotEventBindings)
		{
			FDelegateHandle Handle = CachedASC->RegisterGameplayTagEvent(
				Binding.EventTag, 
				EGameplayTagEventType::AnyCountChange
			).AddUObject(this, &USKLayoutWidgetBase::OnGameplayTagChanged);
			
			if (ASCEventHandles.Contains(Binding.EventTag))
			{
				ASCEventHandles[Binding.EventTag].Add(Handle);
			}
			else
			{
				ASCEventHandles.Add(Binding.EventTag, { Handle });
			}

			ChangeVisibleSlotByTag(Binding.TargetSlotTag, false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Layout] AbilitySystemComponent is null!"));
	}
}

void USKLayoutWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();

	UnregisterTagEvent();
}

void USKLayoutWidgetBase::OnGameplayTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	for (const FSlotEventBinding& Binding : SlotEventBindings)
	{
		if (Binding.EventTag != Tag)
			continue;

		const FGameplayTag& SlotTag = Binding.TargetSlotTag;

		if (NewCount == 1)
		{
			ChangeVisibleSlotByTag(SlotTag, true);
		}
		else if (NewCount == 0)
		{
			ChangeVisibleSlotByTag(SlotTag, false);
		}
		else if (NewCount > 1)
		{
			if (CachedASC && CachedASC->HasMatchingGameplayTag(Tag))
			{
				CachedASC->RemoveLooseGameplayTag(Tag);
			}
		}
	}
}

void USKLayoutWidgetBase::SetSlotData(const TArray<FSlotWidgetData>& NewSlots)
{
	SlotWidgetDataArray = NewSlots;
	NotifySlotDataChanged();
}

//ActiavteWidget을 했을 때 바뀔 것 들을 설정해야 함
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

void USKLayoutWidgetBase::UnregisterTagEvent()
{
	if (!CachedASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Layout] Cannot unregister tag events, CachedASC is null!"));
		return;
	}

	// ASCEventHandles에 등록된 모든 이벤트 해제
	for (auto& Pair : ASCEventHandles)
	{
		FGameplayTag EventTag = Pair.Key;
		TArray<FDelegateHandle>& Handles = Pair.Value;

		for (const FDelegateHandle& Handle : Handles)
		{
			CachedASC->UnregisterGameplayTagEvent(Handle,EventTag, EGameplayTagEventType::AnyCountChange);
		}

		UE_LOG(LogTemp, Log, TEXT("[Layout] Unregistered EventTag: %s (handles: %d)"), *EventTag.ToString(), Handles.Num());
	}

	// 맵 초기화
	ASCEventHandles.Empty();
}

void USKLayoutWidgetBase::ChangeVisibleSlotByTag(FGameplayTag ChangeSlotTag, bool bvisible)
{
	USKSlotBox* ChangeSlot = FindDynamicEntryBoxBySlotTag(ChangeSlotTag);
	if (!ChangeSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("CangeSlot not found"));
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



void USKLayoutWidgetBase::NotifySlotDataChanged() const
{
	OnSlotDataChanged.Broadcast();
}
