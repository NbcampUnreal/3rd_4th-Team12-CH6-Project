// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/SKGameState.h"

#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void ASKGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASKGameState::TestMessage()
{
	if (HasAuthority())
	{
		MulticastTestMessage();
	}
}

void ASKGameState::MulticastTestMessage_Implementation()
{
	UWorld* World = GetWorld();
	if (!World)
		return;
		
	UGameInstance* GameInstance = World->GetGameInstance();
	if(!GameInstance)
		return;
		
	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
		return;

	//메시지 구조체 생성
	FConfirmUIMessage Message;
	Message.Title = FText::FromString(TEXT("테스트1"));
	Message.Message = FText::FromString(TEXT("전역이벤트 테스트"));
	Message.ConfirmText = FText::FromString(TEXT("확인"));
	Message.CancelText = FText::FromString(TEXT("완료"));

	//메시지 전송(채널, 메시지)
	MessageSubsystem->BroadcastMessage(TAG_Message_Channel_RequestConfirm, Message);
}
