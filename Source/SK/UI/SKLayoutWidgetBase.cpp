// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SKLayoutWidgetBase.h"

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
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Layout] AbilitySystemComponent is null!"));
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

//Todo: 추후 Widget 설정에 TargetSlotTag와 TargetPlayerTag를 합쳐서 설정할 수 있는 변수를 넣고
//해당 이벤트를 자동으로 설정하고 TargetPlayerTag가 들어오면 TargetSlotTag의 Slot의 표시 여부를 설정하는 방법으로 바꿀 계획
void USKLayoutWidgetBase::ChangeVisibleSlotByTag(FGameplayTag ChangeSlotTag, bool bvisible)
{
	USKSlotBox* ChangeSlot = FindDynamicEntryBoxBySlotTag(ChangeSlotTag);
	if (!ChangeSlot) return;

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
