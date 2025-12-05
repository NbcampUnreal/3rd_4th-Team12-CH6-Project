// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LayoutWidget/EquipmentMainLayoutWidget.h"

#include "Input/CommonUIInputTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/SKNativeGameplayTags.h"

void UEquipmentMainLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();
 
	EquipMainToInGameHandle = RegisterUIActionBinding(FBindUIActionArgs(EquipMainToInGameData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEquipMainToInGameAction)));
	EquipMainToInGameHandle2 = RegisterUIActionBinding(FBindUIActionArgs(EquipMainToInGameData2, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEquipMainToInGameAction)));
}

void UEquipmentMainLayoutWidget::HandleEquipMainToInGameAction()
{

	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound);
	}
	
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
}
