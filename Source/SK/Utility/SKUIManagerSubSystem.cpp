// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/SKUIManagerSubSystem.h"
#include "UI/SKLayoutWidgetBase.h"
#include "AbilitySystemComponent.h"
#include "GameInstance/SKGameInstance.h"
#include "GameData/UILayoutDataAsset.h"
#include "CommonActivatableWidget.h"
#include "SKGameplayMessageSubsystem.h"
#include "SKNativeGameplayTags.h"

void USKUIManagerSubSystem::CreateLayoutWidget()
{
	if(!SwtichAbleLayoutData) return;

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("LocalPlayer is nullptr"));
		return;
	}
	
	APlayerController* PC = LocalPlayer->GetPlayerController(GetWorld());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController is nullptr"));
		return;
	}
	
	RemoveLayout();
	
	CreateLayoutFromData(SwtichAbleLayoutData, PC);
	
	//기본으로 띄울 UI 태그
	SetLayoutVisibeByTag(TAG_UI_Layout_InGame);

	if (!ConfirmLayoutData)
		return;

	CreateLayoutFromData(ConfirmLayoutData, PC);
}

void USKUIManagerSubSystem::SetLayoutVisibeByTag(FGameplayTag LayoutTag)
{
	if (!LayoutWidgets.Contains(LayoutTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Target layout widget not found: %s"), *LayoutTag.ToString());
		return;
	}

	SetLayoutHiddenByTag(CurrentLayoutTag);
	
	UCommonActivatableWidget* VisibleWidget = LayoutWidgets[LayoutTag];
	if (VisibleWidget)
	{
		VisibleWidget->SetVisibility(ESlateVisibility::Visible);
		VisibleWidget->ActivateWidget();
		CurrentLayoutTag = LayoutTag;
	}
}

void USKUIManagerSubSystem::SetLayoutHiddenByTag(FGameplayTag LayoutTag)
{
	if (!LayoutWidgets.Contains(LayoutTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetLayoutHiddenByTag widget not found: %s"), *LayoutTag.ToString());
	}
	else
	{
		UCommonActivatableWidget* HiddenWidget = LayoutWidgets[LayoutTag];
		if (HiddenWidget)
		{
			HiddenWidget->SetVisibility(ESlateVisibility::Hidden);
			HiddenWidget->DeactivateWidget();
		}
	}
}

void USKUIManagerSubSystem::SettingLayout()
{
	//DataAsset 설정(임시)
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
		return;

	UGameInstance* GI = LocalPlayer->GetGameInstance();
	if (!GI)
		return;
	
	USKGameInstance* MyGI = Cast<USKGameInstance>(GI);
	if (!MyGI)
		return;

	if (MyGI->TestLayoutData)
	{
		SwtichAbleLayoutData = Cast<UUILayoutDataAsset>(MyGI->TestLayoutData.Get());
	}

	if (MyGI->TestConfirmData)
	{
		ConfirmLayoutData = Cast<UUILayoutDataAsset>(MyGI->TestConfirmData.Get());
	}
	
	APlayerController* PC = LocalPlayer->GetPlayerController(GI->GetWorld());
	if (!PC)
		return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
		return;
	
	CreateLayoutWidget();
}

void USKUIManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UWorld* World = GetWorld();
	if (!World)
		return;
		
	UGameInstance* GameInstance = World->GetGameInstance();
	if(!GameInstance)
		return;
		
    USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
		return;

	// 메시지 구독
    LayoutSwitchHandle = MessageSubsystem->RegisterListener<FSwitchLayoutMessage>(
    	TAG_Message_Channel_SwitchLayout,
    	this,
    	&USKUIManagerSubSystem::OnSwitchLayoutMessageReceived
    );

	RequestConfirmHandle = MessageSubsystem->RegisterListener<FConfirmUIMessage>(
		TAG_Message_Channel_RequestConfirm,
		this,
		&USKUIManagerSubSystem::OnRequestConfirmMessageReceived
	);

	ConfirmResponseHandle = MessageSubsystem->RegisterListener<FConfirmResponseMessage>(
		TAG_Message_Channel_ConfirmResponse,
		this,
		&USKUIManagerSubSystem::OnConfirmResponseMessageReceived
	);
}

void USKUIManagerSubSystem::Deinitialize()
{
	RemoveLayout();
	
	Super::Deinitialize();
}

//Layout 제거와 Layout 전환 이벤트 해제
void USKUIManagerSubSystem::RemoveLayout()
{
	LayoutWidgets.Empty();

	if (LayoutSwitchHandle.IsValid())
	{
		LayoutSwitchHandle.Unregister();
	}

	if (RequestConfirmHandle.IsValid())
	{
		RequestConfirmHandle.Unregister();
	}
	
	if (ConfirmResponseHandle.IsValid())
	{
		ConfirmResponseHandle.Unregister();
	}
}

void USKUIManagerSubSystem::CreateLayoutFromData(UUILayoutDataAsset* CreateData, APlayerController* OwningPC)
{
	for (const FLayoutWithSlots& LayoutWithSlots : CreateData->Layouts)
	{
		const FLayoutData& LayoutInfo = LayoutWithSlots.Layout;
 
		if (!LayoutInfo.LayoutWidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("CreateLayoutFromData: LayoutWidgetClass is null for LayoutTag %s"), *LayoutInfo.LayoutTag.ToString());
			continue;
		}
 
		// 레이아웃 위젯 생성
		USKLayoutWidgetBase* LayoutWidget = CreateWidget<USKLayoutWidgetBase>(OwningPC, LayoutInfo.LayoutWidgetClass);
		if (!LayoutWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("CreateLayoutFromData: Failed to create LayoutWidget %s"), *LayoutInfo.LayoutTag.ToString());
			continue;
		}
 
		// 뷰포트에 추가
		LayoutWidget->AddToViewport();
		//해당 Layout의 Slot 데이터 전달
		LayoutWidget->SetSlotData(LayoutWithSlots.Slots, LayoutInfo.LayoutTag);

		if (SwtichAbleLayoutData == CreateData)
		{
			//Map에 현재 레이아웃 태그와 위젯 추가
			LayoutWidgets.Add(LayoutInfo.LayoutTag, LayoutWidget);
		}
		else if (ConfirmLayoutData == CreateData)
		{
			ConfirmLayoutWidgets.Add(LayoutInfo.LayoutTag, LayoutWidget);
		}
	}
}

void USKUIManagerSubSystem::OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("OnSwitchLayoutMessageReceived: Channel = %s | LayoutTag = %s | Visible = %s"),
		*Channel.ToString(),
		*Message.LayoutTag.ToString(),
		Message.bVisible ? TEXT("True") : TEXT("False"));
	
	if (Message.bVisible)
	{
		// 레이아웃 표시
		SetLayoutVisibeByTag(Message.LayoutTag);
	}
	else
	{
		// 레이아웃 숨김
		SetLayoutHiddenByTag(Message.LayoutTag);
	}
	
}

void USKUIManagerSubSystem::OnRequestConfirmMessageReceived(FGameplayTag Channel, const FConfirmUIMessage& Message)
{
	ConfirmLayoutWidgets[TAG_UI_Layout_Confirm]->SetVisibility(ESlateVisibility::Visible);
	ConfirmLayoutWidgets[TAG_UI_Layout_Confirm]->ActivateWidget();
	UE_LOG(LogTemp, Warning, TEXT("OnRequestConfirmMessageReceived"));
}

void USKUIManagerSubSystem::OnConfirmResponseMessageReceived(FGameplayTag Channel,
	const FConfirmResponseMessage& Message)
{
	ConfirmLayoutWidgets[TAG_UI_Layout_Confirm]->SetVisibility(ESlateVisibility::Hidden);
	ConfirmLayoutWidgets[TAG_UI_Layout_Confirm]->DeactivateWidget();
	LayoutWidgets[CurrentLayoutTag]->ActivateWidget();
	UE_LOG(LogTemp, Warning, TEXT("OnConfirmResponseMessageReceived"));
}
