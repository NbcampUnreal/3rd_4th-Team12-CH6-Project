// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/CharacterBuffDebuffSlotWidget.h"

#include "BuffDebuffIconWidget.h"
#include "Components/UniformGridPanel.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/StaticDataSubsystem.h"

void UCharacterBuffDebuffSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TryBind();

	if (BuffIconClass && WidgetPool.Num() == 0)
	{
		for (int32 i = 0; i < MaxPoolSize; i++)
		{
			UBuffDebuffIconWidget* NewWidget = CreateWidget<UBuffDebuffIconWidget>(this, BuffIconClass);
			NewWidget->SetVisibility(ESlateVisibility::Collapsed);
			WidgetPool.Add(NewWidget);
		}
	}
}

void UCharacterBuffDebuffSlotWidget::NativeDestruct()
{
	if (CachedPS)
	{
		CachedPS->OnBuffAdded.RemoveAll(this);
		CachedPS->OnBuffRemoved.RemoveAll(this);
		CachedPS->OnBuffTimeChanged.RemoveAll(this);
		CachedPS->OnBuffStackChanged.RemoveAll(this);
		CachedPS = nullptr;
	}
	
	Super::NativeDestruct();
}

void UCharacterBuffDebuffSlotWidget::TryBind()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) 
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryBind(); });
		return;
	}
 
	APlayerState* PS = PC->GetPlayerState<APlayerState>();
	if (!PS) 
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryBind(); });
		return;
	}

	if (CachedPS)
	{
		CachedPS->OnBuffAdded.RemoveAll(this);
		CachedPS->OnBuffRemoved.RemoveAll(this);
		CachedPS->OnBuffTimeChanged.RemoveAll(this);
		CachedPS->OnBuffStackChanged.RemoveAll(this);
		CachedPS = nullptr;
	}
	
	CachedPS = Cast<ASKPlayerState>(PS);
	if (!CachedPS) 
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryBind(); });
		return;
	}
	
	CachedPS->OnBuffAdded.AddDynamic(this, &UCharacterBuffDebuffSlotWidget::OnBuffAddedHandler);
	CachedPS->OnBuffRemoved.AddDynamic(this, &UCharacterBuffDebuffSlotWidget::OnBuffRemovedHandler);
	CachedPS->OnBuffTimeChanged.AddDynamic(this, &UCharacterBuffDebuffSlotWidget::OnBuffTimeChangedHandler);
	CachedPS->OnBuffStackChanged.AddDynamic(this, &UCharacterBuffDebuffSlotWidget::OnBuffStackChangedHandler);
}

UBuffDebuffIconWidget* UCharacterBuffDebuffSlotWidget::GetWidgetFromPool()
{
	for (UBuffDebuffIconWidget* Widget : WidgetPool)
	{
		if (Widget && Widget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			return Widget;
		}
	}

	// 풀 다 썼다면 생성
	if (BuffIconClass)
	{
		UBuffDebuffIconWidget* NewWidget = CreateWidget<UBuffDebuffIconWidget>(this, BuffIconClass);
		NewWidget->SetVisibility(ESlateVisibility::Collapsed);
		WidgetPool.Add(NewWidget);
		return NewWidget;
	}

	return nullptr;
}

void UCharacterBuffDebuffSlotWidget::ReturnWidgetToPool(UBuffDebuffIconWidget* Widget)
{
	if (!Widget) return;

	Widget->SetVisibility(ESlateVisibility::Collapsed);
}

const FBuffDebuffIconDataTable& UCharacterBuffDebuffSlotWidget::GetIconDataTable(FModifiedAttributeInfo Info)
{
	UWorld* World = GetWorld();
	check(World);

	UGameInstance* GI = World->GetGameInstance();
	check(GI);
	
	UStaticDataSubsystem* SDS = GI->GetSubsystem<UStaticDataSubsystem>();
	check(SDS);

	int32 IconId = 0;
	FString AttrName = Info.Attribute.GetName();

	if (AttrName == "Attack") IconId = 1;
	else if (AttrName == "Armor") IconId = 3;
	else if (AttrName == "Health") IconId = 5;
	else if (AttrName == "Stamina") IconId = 7;
	else if (AttrName == "Heat") IconId = 9;

	check(IconId != 0);

	if (!IsBuff(Info))
	{
		IconId++; // Debuff 아이콘
	}
	
	const FBuffDebuffIconDataTable* WidgetIcon = SDS->GetData<FBuffDebuffIconDataTable>(IconId);
	check(WidgetIcon); // nullptr 불가

	return *WidgetIcon;
}

bool UCharacterBuffDebuffSlotWidget::IsBuff(const FModifiedAttributeInfo& Info)
{
	switch (Info.Op)
	{
	case EGameplayModOp::Additive:
		// +면 Buff, -면 Debuff
		return Info.Magnitude > 0.f;

	case EGameplayModOp::Multiplicitive:
		// 1보다 크면 Buff (배율 증가)
		return Info.Magnitude > 1.f;

	case EGameplayModOp::Division:
		// 1보다 작으면 Buff (/0.5 → 2배 = Buff)
		return Info.Magnitude < 1.f;

	case EGameplayModOp::Override:
		// Override는 기존 값 대비 증가인지 비교가 필요한데
		// 일단 Buff로 처리
		return true;

	default:
		return true;
	}
}

void UCharacterBuffDebuffSlotWidget::AddWidgetToContainer(UBuffDebuffIconWidget* Widget, bool bIsBuff)
{
	UUniformGridPanel* Target = bIsBuff ? BuffContainer: DebuffContainer;
	if (!Target || !Widget) return;

	// 현재 요소 수
	int32 Count = Target->GetChildrenCount();

	int32 Row = Count / MaxColumn;
	int32 Column = Count % MaxColumn;

	Target->AddChildToUniformGrid(Widget, Row, Column);
}

void UCharacterBuffDebuffSlotWidget::RemoveWidgetFromContainer(UBuffDebuffIconWidget* Widget, bool bIsBuff)
{
	if (!Widget) return;

	UUniformGridPanel* Target = bIsBuff ? BuffContainer : DebuffContainer;
	if (!Target) return;

	Target->RemoveChild(Widget);
	Widget->SetVisibility(ESlateVisibility::Collapsed);
	ReturnWidgetToPool(Widget); // 풀로 반환
}

void UCharacterBuffDebuffSlotWidget::RemoveWidgetsFromHandle(FActiveGameplayEffectHandle EffectHandle)
{
	TArray<UBuffDebuffIconWidget*>* WidgetArrayPtr = ActiveWidgets.Find(EffectHandle);
	if (!WidgetArrayPtr) return;

	for (UBuffDebuffIconWidget* Widget : *WidgetArrayPtr)
	{
		if (!Widget) continue;

		bool bIsBuff = Widget->bIsBuff; // 위젯 내부에서 Attribute 기반 판단
		RemoveWidgetFromContainer(Widget, bIsBuff); // Buff/ Debuff 컨테이너 선택
	}

	ActiveWidgets.Remove(EffectHandle);
}

void UCharacterBuffDebuffSlotWidget::OnBuffAddedHandler(
	FActiveGameplayEffectHandle AddEffectHandle,
	const FModifiedAttributeArray& AddModifiedAttributes,
	FGameplayTagContainer AddBuffTags, float Duration)
{
	if (AddModifiedAttributes.Items.Num() == 0)
		return;
	
	for (const FModifiedAttributeInfo& Info : AddModifiedAttributes.Items)
	{
		UBuffDebuffIconWidget* IconWidget = GetWidgetFromPool();
		if (!IconWidget) continue;

		const FBuffDebuffIconDataTable& IconData = GetIconDataTable(Info);
		if (!IconData.StatIcon || !IconData.ModifierIcon)
		{
			ReturnWidgetToPool(IconWidget);
			continue;
		}

		bool bIsBuff = IsBuff(Info);
		int32 StackCount = 1;

		IconWidget->SetupIcon(IconData, Info, Duration, StackCount, bIsBuff);
		IconWidget->SetVisibility(ESlateVisibility::Visible);

		ActiveWidgets.FindOrAdd(AddEffectHandle).Add(IconWidget);

		AddWidgetToContainer(IconWidget, bIsBuff);
	}	
}

void UCharacterBuffDebuffSlotWidget::OnBuffRemovedHandler(FActiveGameplayEffectHandle EffectHandle, FModifiedAttributeArray AddModifiedAttributes)
{
	RemoveWidgetsFromHandle(EffectHandle);
}

void UCharacterBuffDebuffSlotWidget::OnBuffTimeChangedHandler(FActiveGameplayEffectHandle EffectHandle, float NewStartTime, float NewDuration)
{
	TArray<UBuffDebuffIconWidget*>* WidgetArrayPtr = ActiveWidgets.Find(EffectHandle);
	if (!WidgetArrayPtr) return;

	for (UBuffDebuffIconWidget* Widget : *WidgetArrayPtr)
	{
		if (!Widget) continue;

		// 위젯에서 남은 시간 업데이트
		Widget->SetDuration(NewDuration);
	}
}

void UCharacterBuffDebuffSlotWidget::OnBuffStackChangedHandler(FActiveGameplayEffectHandle EffectHandle, int32 NewStack, int32 OldStack)
{
	TArray<UBuffDebuffIconWidget*>* WidgetArrayPtr = ActiveWidgets.Find(EffectHandle);
	if (!WidgetArrayPtr) return;

	for (UBuffDebuffIconWidget* Widget : *WidgetArrayPtr)
	{
		if (!Widget) continue;

		// 위젯에서 스택 수 업데이트
		Widget->SetStackCount(NewStack);
	}
}
