// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/ConfirmSlotWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Utility/SKUIManagerSubSystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void UConfirmSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UConfirmSlotWidget::HandleConfirmClicked);
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UConfirmSlotWidget::HandleCancelClicked);
	}

	NormalStyle = ConfirmButton->GetStyle();

	UWorld* World = GetWorld();
	if (!World)
		return;
		
	UGameInstance* GameInstance = World->GetGameInstance();
	if(!GameInstance)
		return;
		
	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
		return;

	RequestConfirmHandle = MessageSubsystem->RegisterListener<FConfirmUIMessage>(
		TAG_Message_Channel_RequestConfirm,
		this,
		&UConfirmSlotWidget::OnRequestConfirmMessageReceived
	);

	InteractionHandle = MessageSubsystem->RegisterListener<FUIInteractionMoveMessage>(
		TAG_Message_Channel_UIInteraction,
		this,
		&UConfirmSlotWidget::OnInteractionMessageReceived
	);
}

void UConfirmSlotWidget::NativeDestruct()
{
	if (RequestConfirmHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(RequestConfirmHandle);
		}
	}

	if (InteractionHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(InteractionHandle);
		}
	}
	
	Super::NativeDestruct();
}

void UConfirmSlotWidget::HandleConfirmClicked()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FConfirmResponseMessage Message;

			// SlotTag를 Confirm UI 동작의 식별자로 사용
			Message.SlotTag = RequestSlotTag;
			Message.bAccepted = true;
			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_ConfirmResponse, Message);
		}
	}

	RequestSlotTag = FGameplayTag();
}

void UConfirmSlotWidget::HandleCancelClicked()
{
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FConfirmResponseMessage Message;

			// SlotTag를 Confirm UI 동작의 식별자로 사용
			Message.SlotTag = RequestSlotTag;
			Message.bAccepted = false;
			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_ConfirmResponse, Message);
		}
	}

	RequestSlotTag = FGameplayTag();
}

void UConfirmSlotWidget::SimulateButtonHover(UButton* Button)
{
	if (!Button) return;
 
	TSharedPtr<SWidget> SlateWidget = Button->GetCachedWidget();
	if (!SlateWidget.IsValid())
		return;
	
	TSharedPtr<SButton> SlateButton = StaticCastSharedPtr<SButton>(SlateWidget);
	if (!SlateButton.IsValid()) return;

	// 기본 생성자로 임시 Geometry와 PointerEvent 생성
	FGeometry DummyGeometry;
	FPointerEvent DummyPointerEvent;

	// Slate의 OnMouseEnter 호출하여 호버 이벤트 시뮬레이션
	SlateButton->OnMouseEnter(DummyGeometry, DummyPointerEvent);
 
	// 필요하다면 UMG OnHovered 이벤트도 수동 실행
	Button->OnHovered.Broadcast();
}

void UConfirmSlotWidget::SimulateButtonUnHover(UButton* Button)
{
	if (!Button)
	{
		UE_LOG(LogTemp, Warning, TEXT("Button is null!"));
		return;
	}
 
	TSharedPtr<SWidget> SlateWidget = Button->GetCachedWidget();
	if (!SlateWidget.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SlateWidget is invalid!"));
		return;
	}

 
	TSharedPtr<SButton> SlateButton = StaticCastSharedPtr<SButton>(SlateWidget);
	if (!SlateButton.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SlateButton is invalid!"));
		return;
	}
	
	FPointerEvent DummyPointerEvent;
	
	// 언호버 시뮬레이션
	SlateButton->OnMouseLeave(DummyPointerEvent);
	Button->OnUnhovered.Broadcast();
}

void UConfirmSlotWidget::ButtonMove(int32 Direction)
{
	// 현재 선택된 인덱스
	int32 CurrentIndex = CurrentSelectButton; // 멤버 변수라고 가정 (0 또는 1)

	int32 Offset = 0;

	if (Direction == 2)
	{
		Offset = -1;
	}
	else if (Direction == 3)
	{
		Offset = 1;
	}
	else
	{
		return; // 2,3 외 입력 무시
	}

	int32 NewIndex = CurrentIndex + Offset;

	// 0과 1로 Clamp
	NewIndex = FMath::Clamp(NewIndex, 0, 1);

	// 변경이 있을 때만 업데이트
	if (NewIndex != CurrentIndex)
	{
		
		if (NewIndex == 0)
		{
			SimulateButtonHover(ConfirmButton);
			SimulateButtonUnHover(CancelButton);
		}
		else
		{
			SimulateButtonHover(CancelButton);
			SimulateButtonUnHover(ConfirmButton);
		}
		CurrentSelectButton = NewIndex;
	}
}

void UConfirmSlotWidget::OnRequestConfirmMessageReceived(FGameplayTag Channel, const FConfirmUIMessage& Message)
{	
	if (TitleText)
	{
		TitleText->SetText(Message.Title);
	}

	if (MessageText)
	{
		MessageText->SetText(Message.Message);
	}

	if (ConfirmButtonText)
	{
		ConfirmButtonText->SetText(Message.ConfirmText);
	}

	if (CancelButtonText)
	{
		CancelButtonText->SetText(Message.CancelText);
	}

	RequestSlotTag = Message.SlotTag;
}

void UConfirmSlotWidget::OnInteractionMessageReceived(FGameplayTag Channel, const FUIInteractionMoveMessage& Message)
{
	if (Message.Type != EUIMessageType::Confirm)
	{
		return;
	}

	switch (Message.MoveDirection)
	{
		case 2:
			ButtonMove(2);
			break;
		case 3:
			ButtonMove(3);
			break;
		case 4:
			if (CurrentSelectButton == 0)
			{
				HandleConfirmClicked();
			}
			else
			{
				HandleCancelClicked();
			}
			break;
		case 5:
			HandleCancelClicked();
			break;
		default:
			return;
	}
	
}

