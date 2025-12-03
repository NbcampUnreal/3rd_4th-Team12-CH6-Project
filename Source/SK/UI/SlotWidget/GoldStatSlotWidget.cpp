// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/GoldStatSlotWidget.h"

#include "Components/TextBlock.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "PlayerState/SKPlayerState.h"

void UGoldStatSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TryBind();
}

void UGoldStatSlotWidget::NativeDestruct()
{
	if (AttributeSet)
	{
		AttributeSet->OnGoldChanged.RemoveAll(this);
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
	
	ASKPlayerState* CurrentPS = Cast<ASKPlayerState>(PS);
	if (!CurrentPS) 
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryBind(); });
		return;
	}
	
	UAbilitySystemComponent* ASC = CurrentPS->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryBind(); });
		return;
	}
 
	// 기존 바인딩 해제
	if (AttributeSet)
	{
		AttributeSet->OnGoldChanged.RemoveAll(this);
		AttributeSet = nullptr;
	}
 
	AttributeSet = Cast<USKAttributeSet>(ASC->GetAttributeSet(USKAttributeSet::StaticClass()));
	if (!AttributeSet)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { TryBind(); });
		return;
	}
 
	AttributeSet->OnGoldChanged.AddUObject(this, &UGoldStatSlotWidget::GlodChanged); 
 
	// 초기 UI 업데이트
	GlodChanged(nullptr, nullptr, nullptr, 0.f, 0.f, AttributeSet->GetHealth());
}

void UGoldStatSlotWidget::GlodChanged(AActor* EffectInstigator, AActor* EffectCauser,
                                      const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	    const float Delta = EffectMagnitude; // 증가한 골드량

    // 애니매이션 재생 중이면 → 증가량 누적 후 다시 표시만 업데이트
    if (bIsPlayingAnimation)
    {
        PendingGold += Delta;
    	PlusGoldText->SetText(FText::FromString(FString::Printf(TEXT("+ %.0f"), PendingGold)));
        return;
    }

    // 애니매이션이 안 돌고 있었다면 새로 시작
    PendingGold = Delta;
	PlusGoldText->SetText(FText::FromString(FString::Printf(TEXT("+ %.0f"), PendingGold)));


	if (PendingGold <= 0.0f)
	{
		OnGoldAddAnimationFinished();
		return;
	}
	
    // 애니 종료 이벤트 바인딩
    FWidgetAnimationDynamicEvent End;
    End.BindDynamic(this, &UGoldStatSlotWidget::OnGoldAddAnimationFinished);
    BindToAnimationFinished(GoldAddAnimation, End);

    bIsPlayingAnimation = true;

    PlayAnimation(GoldAddAnimation);
}

void UGoldStatSlotWidget::OnGoldAddAnimationFinished()
{
	// TotalGoldText 업데이트 (실제 NewValue를 넘겨받을 수 없는 경우 OldValue+Pending 방식도 가능)
	const float CurrentTotal = FCString::Atof(*TotalGoldText->GetText().ToString());
	const float NewTotal = CurrentTotal + PendingGold;

	TotalGoldText->SetText(FText::AsNumber(NewTotal));

	// 상태 초기화
	PendingGold = 0.0f;
	bIsPlayingAnimation = false;
}
