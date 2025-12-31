// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/BonfireMenuSlotWidget.h"

#include "Character/SKPlayerCharacter.h"
#include "Component/QuickSlotComponent.h"
#include "Components/Button.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"
#include "AbilitySystemComponent.h"

void UBonfireMenuSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MenuButtons = {
		BtnLevelUp,
		BtnRest,
		BtnLeave
	};

	if (BtnLevelUp)
		BtnLevelUp->OnClicked.AddDynamic(this, &UBonfireMenuSlotWidget::OnLevelUpClicked);
	
	if (BtnRest)
		BtnRest->OnClicked.AddDynamic(this, &UBonfireMenuSlotWidget::OnRestClicked);

	if (BtnLeave)
	{
		BtnLeave->OnClicked.AddDynamic(this, &UBonfireMenuSlotWidget::OnLeaveClicked);
		BtnLeave->OnHovered.AddDynamic(this, &UBonfireMenuSlotWidget::OnLeaveHovered);
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
	{
		return;
	}

	MenuMoveHandle = MessageSubsystem->RegisterListener<FUIInteractionMoveMessage>(
		TAG_Message_Channel_UIInteraction,
		this,
		&UBonfireMenuSlotWidget::OnMenuMoveMessageReceived
	);

	NormalStyle = BtnLevelUp->GetStyle();
}

void UBonfireMenuSlotWidget::NativeDestruct()
{
	if (MenuMoveHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(MenuMoveHandle);
		}
	}
	
	Super::NativeDestruct();
}

void UBonfireMenuSlotWidget::OnMenuMoveMessageReceived(FGameplayTag Channel, const FUIInteractionMoveMessage& Message)
{
	if (Message.Type != EUIMessageType::BonfireMove)
	{
		return;
	}

	switch (Message.MoveDirection)
	{
		case 0: MoveSelectionUp(); break;
		case 1: MoveSelectionDown(); break;
		case 4: PressCurrentButton(); break;
	};
}

void UBonfireMenuSlotWidget::PressCurrentButton()
{
	if (!MenuButtons.IsValidIndex(CurrentIndex)) return;

	if (UButton* CurButton = MenuButtons[CurrentIndex])
	{
		CurButton->OnClicked.Broadcast();
	}
}

void UBonfireMenuSlotWidget::OnLevelUpClicked()
{
	UE_LOG(LogTemp, Log, TEXT("LevelUp 버튼 클릭"));
	// TODO : 레벨업 UI 호출

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;
	
	ASKPlayerState* PS = PC->GetPlayerState<ASKPlayerState>();
	if (!PS) return;

	PS->Server_RequestLevelUp();
}

void UBonfireMenuSlotWidget::OnRestClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Rest 버튼 클릭"));
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ASKPlayerState* PS = PC->GetPlayerState<ASKPlayerState>();
	if (!PS) return;

	UQuickSlotComponent* QSComponent = PS->FindComponentByClass<UQuickSlotComponent>();
	if (!QSComponent) return;

	if (PC->HasAuthority())
	{
		QSComponent->RefreshQuickSlots();
	}
	else
	{
		QSComponent->ServerRefreshQuickSlots();
	}
	
}

void UBonfireMenuSlotWidget::OnLeaveClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Leave 버튼 클릭"));
	if (UWorld* World = GetWorld())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(World))
		{
			// 전송할 메시지 생성
			FSwitchLayoutMessage Message(TAG_UI_Layout_InGame, true);

			// 메시지 브로드캐스트 (UI 전환용 채널로)
			MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SwitchLayout, Message);

			UE_LOG(LogTemp, Log, TEXT("Broadcast SwitchLayout Message: %s"), *Message.LayoutTag.ToString());
		}
	}
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;
	
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(PC->GetPawn());
	if (!PlayerCharacter) return;
	
	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayTagContainer BonfireEndTag;
	BonfireEndTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.InteractBonfireEnd")));

	ASC->TryActivateAbilitiesByTag(BonfireEndTag);

	
}

void UBonfireMenuSlotWidget::OnLeaveHovered()
{
	UE_LOG(LogTemp, Warning, TEXT("Test Hover"));
	CurrentIndex = 2;
}

void UBonfireMenuSlotWidget::SimulateButtonHover(UButton* Button)
{
	if (!Button) return;
 
	TSharedPtr<SWidget> SlateWidget = Button->GetCachedWidget();
	if (SlateWidget.IsValid())
	{
		TSharedPtr<SButton> SlateButton = StaticCastSharedPtr<SButton>(SlateWidget);
		if (SlateButton.IsValid())
		{
			// 기본 생성자로 임시 Geometry와 PointerEvent 생성
			FGeometry DummyGeometry;
			FPointerEvent DummyPointerEvent;
 
			// Slate의 OnMouseEnter 호출하여 호버 이벤트 시뮬레이션
			SlateButton->OnMouseEnter(DummyGeometry, DummyPointerEvent);
 
			// 필요하다면 UMG OnHovered 이벤트도 수동 실행
			Button->OnHovered.Broadcast();
		}
	}
}

void UBonfireMenuSlotWidget::SimulateButtonUnHover(UButton* Button)
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
 
	// 더미 Geometry와 PointerEvent 생성
	FGeometry DummyGeometry;
	FPointerEvent DummyPointerEvent;
	
	// 언호버 시뮬레이션
	SlateButton->OnMouseLeave(DummyPointerEvent);
	Button->OnUnhovered.Broadcast();
}


void UBonfireMenuSlotWidget::MoveSelectionUp()
{
	if (MenuButtons.Num() == 0) return;

	int32 OldIndex = CurrentIndex;
	CurrentIndex = (CurrentIndex - 1 + MenuButtons.Num()) % MenuButtons.Num();
	ApplyFocusToButton(OldIndex, CurrentIndex);
}

void UBonfireMenuSlotWidget::MoveSelectionDown()
{
	if (MenuButtons.Num() == 0) return;

	int32 OldIndex = CurrentIndex;
	CurrentIndex = (CurrentIndex + 1) % MenuButtons.Num();
	ApplyFocusToButton(OldIndex, CurrentIndex);
}

void UBonfireMenuSlotWidget::ApplyFocusToButton(int32 OldIndex, int32 NewIndex)
{
	if (!MenuButtons.IsValidIndex(NewIndex)) return;

	// 이전 버튼 Unhover
	if (MenuButtons.IsValidIndex(OldIndex) && MenuButtons[OldIndex])
	{
		SimulateButtonUnHover(MenuButtons[OldIndex]);
	}

	// 새로운 버튼 Hover
	if (UButton* NewButton = MenuButtons[NewIndex])
	{
		SimulateButtonHover(NewButton);
	}
}

