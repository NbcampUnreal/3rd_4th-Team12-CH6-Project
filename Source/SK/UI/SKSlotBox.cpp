// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SKSlotBox.h"
#include "UI/SKLayoutWidgetBase.h"
#include "Blueprint/UserWidget.h"

void USKSlotBox::InitializeSlotBox(UUserWidget* InParent)
{
	if (IsDesignTime())
	{
		return; // 에디터 디자인 타임/검증 중에는 실행하지 않음
	}
	ParentLayoutWidget = Cast<USKLayoutWidgetBase>(InParent);
	if (ParentLayoutWidget)
	{
		// 델리게이트 중복 바인딩 방지 후 등록
		ParentLayoutWidget->OnSlotDataChanged.RemoveAll(this);
		ParentLayoutWidget->OnSlotDataChanged.AddDynamic(this, &USKSlotBox::OnSlotDataChanged);
	}
	UE_LOG(LogTemp, Warning, TEXT("InitializeSlotBox"));
}

void USKSlotBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (UUserWidget* ParentWidget = GetTypedOuter<UUserWidget>())
	{
		InitializeSlotBox(ParentWidget);
	}
	InitEntrySizeRule(ESlateSizeRule::Fill);
}

void USKSlotBox::OnSlotDataChanged()
{
	SettingSlot();
}

void USKSlotBox::SettingSlot()
{
	if (!ParentLayoutWidget)
	{
		return;
	}

	const TArray<FSlotWidgetData>& AllSlots = ParentLayoutWidget->GetSlotData();
 
	for (const FSlotWidgetData& SlotData : AllSlots)
	{
		if (SlotData.SlotTag == BoxSlotTag && SlotData.WidgetClass)
		{
			if (CurrentSlotWidget && CurrentSlotWidget->GetClass() == SlotData.WidgetClass)
			{
				UE_LOG(LogTemp, Log, TEXT("[SlotBox] 중복 생성 방지"));
				return;
			}
			// 기존 엔트리 모두 제거 (슬레이트 위젯 완전 삭제)
			ResetInternal(true);
			UUserWidget* NewWidget = CreateEntryInternal(SlotData.WidgetClass);
			CurrentSlotWidget = NewWidget;
			return;
		}
	}

	ResetInternal(true);
	CurrentSlotWidget = nullptr;
	return;
}

TSharedRef<SWidget> USKSlotBox::RebuildWidget()
{
	if (IsDesignTime())
	{
		TSharedRef<SOverlay> OverlayWidget = SNew(SOverlay);
 
		OverlayWidget->AddSlot()
			.Padding(5.0f)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Text(FText::Format(
					NSLOCTEXT("SKSlotBox", "DesignTimeTag", "SlotBox Tag:\n{0}"),
					FText::FromName(BoxSlotTag.GetTagName())
				))
			];
 
		return OverlayWidget;
	}
	else
	{
		return Super::RebuildWidget();
	}
}
