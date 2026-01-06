// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/GameDirectionSlotWidget.h"

#include "Components/TextBlock.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UGameDirectionSlotWidget::OnGameDirectionMessageReceived(FGameplayTag Channel,
                                                              const FGameDirectionMessage& Message)
{
	if (Direction < Message.Direction)
	{
		UpdateDirectionText(Message.Direction);
	}
}

void UGameDirectionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this);
	if (!MessageSubsystem)
		return;

	GameDirectionHandle = MessageSubsystem->RegisterListener<FGameDirectionMessage>(
		TAG_Message_Channel_GameDirection,
		this,
		&UGameDirectionSlotWidget::OnGameDirectionMessageReceived
	);
	
	TryBind();
}

void UGameDirectionSlotWidget::NativeDestruct()
{
	if (GameDirectionHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(GameDirectionHandle);
		}
	}
	
	Super::NativeDestruct();
}

void UGameDirectionSlotWidget::TryBind()
{
	APlayerState* TempPS = GetOwningPlayerState();
	if (!TempPS)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimerForNextTick(this, &UGameDirectionSlotWidget::TryBind);
		}
		return;
	}

	CurrentPS = Cast<ASKPlayerState>(TempPS);
	if (!CurrentPS)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimerForNextTick(this, &UGameDirectionSlotWidget::TryBind);
		}
		return;
	}

	Direction = CurrentPS->GetGameDirection();

	UpdateDirectionText(Direction);
}

void UGameDirectionSlotWidget::UpdateDirectionText(int32 NewDirection)
{
	if (!DirectionText)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGameDirectionSlotWidget::UpdateDirectionText - DirectionText is null"));
		return;
	}

	if (!ChangeDirectionAnim)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGameDirectionSlotWidget::UpdateDirectionText - ChangeDirectionAnim is null"));
		return;
	}

	if (Direction == NewDirection)
		return;

	Direction = NewDirection;
	
	for (const auto& Mapping : DirectionTextMappings)
	{
		if (Mapping.Value == Direction)
		{
			DirectionText->SetText(Mapping.DisplayText);
			break;
		}
	}
	
}
