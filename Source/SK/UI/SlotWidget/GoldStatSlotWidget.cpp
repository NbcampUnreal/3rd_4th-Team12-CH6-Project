// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/GoldStatSlotWidget.h"

#include "Components/TextBlock.h"
#include "PlayerState/SKPlayerState.h"

void UGoldStatSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TryBind();
}

void UGoldStatSlotWidget::NativeDestruct()
{
	if (CurrentPS)
	{
		CurrentPS->GoldChanged.RemoveDynamic(this, &UGoldStatSlotWidget::GoldChanged);
	}
	
	Super::NativeDestruct();
}

void UGoldStatSlotWidget::TryBind()
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
	
	CurrentPS = Cast<ASKPlayerState>(PS);
	if (!CurrentPS) 
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryBind(); });
		return;
	}
	
	CurrentPS->GoldChanged.RemoveDynamic(this, &UGoldStatSlotWidget::GoldChanged);
	CurrentPS->GoldChanged.AddDynamic(this, &UGoldStatSlotWidget::GoldChanged);

	OnGoldAddAnimationFinished();
}

void UGoldStatSlotWidget::GoldChanged(int32 NewGold, int32 OldGold)
{
	int32 Delta = NewGold - OldGold; // 증가한 골드량

	// 애니매이션 재생 중이면 → 증가량 누적 후 다시 표시만 업데이트
	if (bIsPlayingAnimation)
	{
		PendingGold += Delta;
		PlusGoldText->SetText(FText::FromString(FString::Printf(TEXT("+ %d"), PendingGold)));
		return;
	}

	// 애니매이션이 안 돌고 있었다면 새로 시작
	PendingGold = Delta;
	PlusGoldText->SetText(FText::FromString(FString::Printf(TEXT("+ %d"), PendingGold)));

	// 증가량 없으면 애니 없이 종료 처리
	if (PendingGold <= 0)
	{
		OnGoldAddAnimationFinished();
		return;
	}

	// 애니메이션 종료 이벤트 바인딩
	FWidgetAnimationDynamicEvent End;
	End.BindDynamic(this, &UGoldStatSlotWidget::OnGoldAddAnimationFinished);
	BindToAnimationFinished(GoldAddAnimation, End);

	bIsPlayingAnimation = true;

	PlayAnimation(GoldAddAnimation);
}

void UGoldStatSlotWidget::OnGoldAddAnimationFinished()
{
	int32 NewTotal = CurrentPS->GetGold();

	// Total UI 갱신 (int32)
	TotalGoldText->SetText(FText::AsNumber(NewTotal));

	// 상태 초기화
	PendingGold = 0;
	bIsPlayingAnimation = false;
}
