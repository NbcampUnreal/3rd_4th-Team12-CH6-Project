// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TitleWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameInstance/SKGameInstance.h"
#include "Input/CommonUIInputTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UTitleWidget::UTitleWidget()
{
}

void UTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (NewGameButton)
	{
		NewGameButton->OnClicked.AddDynamic(this, &UTitleWidget::OnNewGameClicked);
	}

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UTitleWidget::OnContinueClicked);
	}

	if (OptionButton)
	{
		OptionButton->OnClicked.AddDynamic(this, &UTitleWidget::OnOptionClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UTitleWidget::OnExitClicked);
	}

	MenuButtons = {
		NewGameButton,
		ContinueButton,
		OptionButton,
		ExitButton
	};

	CurrentSelectedIndex = 0;
	ApplyFocusToButton(0, 0);
	
	if (FadeOutAnim)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &UTitleWidget::OnFadeOutFinished);
		BindToAnimationFinished(FadeOutAnim, EndEvent);
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->bShowMouseCursor = true;
		PC->SetShowMouseCursor(true); // 둘 다
	}

	MenuUPHandle = RegisterUIActionBinding(FBindUIActionArgs(MenuUPData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMeneUpAction)));
	MenuDownHandle = RegisterUIActionBinding(FBindUIActionArgs(MenuDownData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMenuDownAction)));
	MenuSelectHandle = RegisterUIActionBinding(FBindUIActionArgs(MenuSelectData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMenuSelectAction)));
	MenuBackHandle = RegisterUIActionBinding(FBindUIActionArgs(MenuBackData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMenuBackAction)));

	if (FadeInAnim)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &UTitleWidget::OnFadeInFinished);
		BindToAnimationFinished(FadeInAnim, EndEvent);

		PlayAnimation(FadeInAnim);
	}
}

void UTitleWidget::NativeDestruct()
{
	if (NewGameButton)
		NewGameButton->OnClicked.RemoveAll(this);

	if (ContinueButton)
		ContinueButton->OnClicked.RemoveAll(this);

	if (OptionButton)
		OptionButton->OnClicked.RemoveAll(this);

	if (ExitButton)
		ExitButton->OnClicked.RemoveAll(this);
	
	Super::NativeDestruct();
}

TOptional<FUIInputConfig> UTitleWidget::GetDesiredInputConfig() const
{
	FUIInputConfig Config(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture, false);

	return Config;
}

void UTitleWidget::OnNewGameClicked()
{
	if (!FadeOutAnim)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TitleWidget] FadeOutAnim missing, start immediately"));
		if (USKGameInstance* GI = GetGameInstance<USKGameInstance>())
		{
			GI->StartGameFromTitle();
		}
		return;
	}

	PlayAnimation(FadeOutAnim);
}

void UTitleWidget::OnFadeOutFinished()
{
	if (!bCanInteract)
		return;
	
	if (USKGameInstance* GI = GetGameInstance<USKGameInstance>())
	{
		GI->StartGameFromTitle();
	}
}

void UTitleWidget::OnFadeInFinished()
{
	bCanInteract = true;
	if (ContinueButton)
	{
		ContinueButton->SetIsEnabled(false);
	}
}

void UTitleWidget::OnContinueClicked()
{
	if (!bCanInteract)
		return;
	
	UE_LOG(LogTemp, Log, TEXT("Continue Clicked"));
	// TODO: SaveGame 체크
}

void UTitleWidget::OnOptionClicked()
{
	if (!bCanInteract)
		return;
	
	UE_LOG(LogTemp, Log, TEXT("Option Clicked"));
	if (!OptionRoot)
	{
		UE_LOG(LogTemp, Warning, TEXT("OptionWidgetInstance is null"));
		return;
	}

	bOptionOpened = !bOptionOpened;

	OptionRoot->SetVisibility(
		bOptionOpened ? ESlateVisibility::Visible : ESlateVisibility::Hidden
	);
	
	if (OptionText)
	{
		OptionText->SetVisibility(
			bOptionOpened ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}

	if (OptionImage)
	{
		OptionImage->SetVisibility(
			bOptionOpened ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}
}

void UTitleWidget::OnExitClicked()
{
	if (!bCanInteract)
		return;
	
	UKismetSystemLibrary::QuitGame(
		this,
		nullptr,
		EQuitPreference::Quit,
		false
	);
}

void UTitleWidget::HandleMeneUpAction()
{
	if (!bCanInteract)
		return;
	
	if (bOptionOpened)
		return;

	if (MenuButtons.Num() <= 1) 
		return;

	const int32 OldIndex = CurrentSelectedIndex;
	int32 NewIndex = CurrentSelectedIndex;

	do
	{
		NewIndex = (NewIndex - 1 + MenuButtons.Num()) % MenuButtons.Num();
	}
	while (NewIndex == 1);

	CurrentSelectedIndex = NewIndex;
	ApplyFocusToButton(OldIndex, CurrentSelectedIndex);
	PlayMenuMoveSound();
}

void UTitleWidget::HandleMenuDownAction()
{
	if (!bCanInteract)
		return;
	
	if (bOptionOpened)
		return;

	if (MenuButtons.Num() <= 1) 
		return;

	const int32 OldIndex = CurrentSelectedIndex;
	int32 NewIndex = CurrentSelectedIndex;

	do
	{
		NewIndex = (NewIndex + 1) % MenuButtons.Num();
	}
	while (NewIndex == 1);

	CurrentSelectedIndex = NewIndex;
	ApplyFocusToButton(OldIndex, CurrentSelectedIndex);
	PlayMenuMoveSound();
}

void UTitleWidget::HandleMenuBackAction()
{
	if (!bCanInteract)
		return;
	
	UE_LOG(LogTemp, Log, TEXT("HandleMenuBackAction"));
	if (bOptionOpened)
	{
		OnOptionClicked();
	}
}

void UTitleWidget::HandleMenuSelectAction()
{
	if (!bCanInteract)
		return;
	
	if (bOptionOpened)
		return;
	
	if (MenuButtons.Num() == 0) return;
	
	if (!MenuButtons.IsValidIndex(CurrentSelectedIndex)) return;

	UButton* Button = MenuButtons[CurrentSelectedIndex];
	if (!Button) return;

	// 1️⃣ Pressed 상태 진입
	SimulateButtonPress(Button);

	// 2️⃣ 일정 시간 후 Release + Click
	GetWorld()->GetTimerManager().SetTimer(
		PressedHoldTimer,
		FTimerDelegate::CreateUObject(
			this,
			&UTitleWidget::SimulateButtonRelease,
			Button
		),
		PressedHoldTime,
		false
	);
}

void UTitleWidget::ApplyFocusToButton(int32 OldIndex, int32 NewIndex)
{
	if (!MenuButtons.IsValidIndex(NewIndex))
		return;

	// 이전 버튼 Unhover
	if (MenuButtons.IsValidIndex(OldIndex))
	{
		if (UButton* OldButton = MenuButtons[OldIndex])
		{
			SimulateButtonUnHover(OldButton);
		}
	}
	
	// 새 버튼 Hover
	if (UButton* NewButton = MenuButtons[NewIndex])
	{
		SimulateButtonHover(NewButton);
	}
}

void UTitleWidget::SimulateButtonHover(UButton* Button)
{
	if (!Button) return;

	TSharedPtr<SWidget> SlateWidget = Button->GetCachedWidget();
	if (!SlateWidget.IsValid()) return;

	TSharedPtr<SButton> SlateButton = StaticCastSharedPtr<SButton>(SlateWidget);
	if (!SlateButton.IsValid()) return;

	FGeometry DummyGeometry;
	FPointerEvent DummyPointerEvent;

	SlateButton->OnMouseEnter(DummyGeometry, DummyPointerEvent);
}

void UTitleWidget::SimulateButtonUnHover(UButton* Button)
{
	if (!Button) return;

	TSharedPtr<SWidget> SlateWidget = Button->GetCachedWidget();
	if (!SlateWidget.IsValid()) return;

	TSharedPtr<SButton> SlateButton = StaticCastSharedPtr<SButton>(SlateWidget);
	if (!SlateButton.IsValid()) return;

	FPointerEvent DummyPointerEvent;
	SlateButton->OnMouseLeave(DummyPointerEvent);
}

void UTitleWidget::SimulateButtonPress(UButton* Button)
{
	if (!Button) return;

	TSharedPtr<SWidget> SlateWidget = Button->GetCachedWidget();
	if (!SlateWidget.IsValid()) return;

	TSharedPtr<SButton> SlateButton = StaticCastSharedPtr<SButton>(SlateWidget);
	if (!SlateButton.IsValid()) return;

	FGeometry DummyGeometry;

	FPointerEvent MouseDownEvent(
		0,
		FVector2D::ZeroVector,
		FVector2D::ZeroVector,
		TSet<FKey>{ EKeys::LeftMouseButton },
		EKeys::LeftMouseButton,
		0,
		FModifierKeysState()
	);

	SlateButton->OnMouseButtonDown(DummyGeometry, MouseDownEvent);
}

void UTitleWidget::SimulateButtonRelease(UButton* Button)
{
	if (!Button) return;

	TSharedPtr<SWidget> SlateWidget = Button->GetCachedWidget();
	if (!SlateWidget.IsValid()) return;

	TSharedPtr<SButton> SlateButton = StaticCastSharedPtr<SButton>(SlateWidget);
	if (!SlateButton.IsValid()) return;

	FGeometry DummyGeometry;

	FPointerEvent MouseUpEvent(
		0,
		FVector2D::ZeroVector,
		FVector2D::ZeroVector,
		TSet<FKey>{},
		EKeys::LeftMouseButton,
		0,
		FModifierKeysState()
	);

	SlateButton->OnMouseButtonUp(DummyGeometry, MouseUpEvent);
	Button->OnClicked.Broadcast();
}

void UTitleWidget::PlayMenuMoveSound()
{
	if (MenuMoveSound)
	{
		UGameplayStatics::PlaySound2D(this, MenuMoveSound);
	}
	
}
