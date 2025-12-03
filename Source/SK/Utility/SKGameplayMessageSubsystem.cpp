#include "Utility/SKGameplayMessageSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "StructUtils/InstancedStruct.h"

DEFINE_LOG_CATEGORY_STATIC(LogSKGameplayMessageSubsystem, Log, All);

void FSKGameplayMessageListenerHandle::Unregister()
{
	if (Subsystem.IsValid())
	{
		Subsystem->UnregisterListener(*this);
	}
}

void USKGameplayMessageSubsystem::BroadcastMessage_BP(FGameplayTag Channel, const FInstancedStruct& Message)
{
	if (!Message.IsValid())
		return;

	const UScriptStruct* StructType = Message.GetScriptStruct();
	const void* StructData = Message.GetMemory();

	BroadcastMessageInternal(Channel, StructType, StructData);
}

void USKGameplayMessageSubsystem::BroadcastMessageInternal(
	FGameplayTag Channel, const UScriptStruct* StructType, const void* MessageBytes)
{
	if (!Channel.IsValid() || !StructType)
	{
		UE_LOG(LogSKGameplayMessageSubsystem, Warning, TEXT("Invalid BroadcastMessageInternal call."));
		return;
	}

	for (const TPair<FGameplayTag, FChannelListenerList>& Pair : ListenerMap)
	{
		const FGameplayTag& RegisteredChannel = Pair.Key;

		// 정확히 일치 또는 부분 일치인지 검사
		bool bMatches = false;
		if (RegisteredChannel.MatchesTagExact(Channel))
		{
			bMatches = true;
		}
		else if (Channel.MatchesTag(RegisteredChannel))
		{
			// PartialMatch 용
			for (const FSKGameplayMessageListenerData& ListenerData : Pair.Value.Listeners)
			{
				if (ListenerData.MatchType == ESKGameplayMessageMatch::PartialMatch)
				{
					bMatches = true;
					break;
				}
			}
		}

		if (!bMatches)
			continue;

		// 실제 콜백 호출
		for (const FSKGameplayMessageListenerData& ListenerData : Pair.Value.Listeners)
		{
			if (ListenerData.ReceivedCallback)
			{
				ListenerData.ReceivedCallback(Channel, StructType, MessageBytes);
			}
		}
	}

	UE_LOG(LogSKGameplayMessageSubsystem, Verbose, TEXT("Broadcasted message on: %s"), *Channel.ToString());
}

FSKGameplayMessageListenerHandle USKGameplayMessageSubsystem::RegisterListenerInternal(
	FGameplayTag Channel,
	TFunction<void(FGameplayTag, const UScriptStruct*, const void*)>&& Callback,
	const UScriptStruct* StructType,
	ESKGameplayMessageMatch MatchType)
{
	if (!Channel.IsValid())
	{
		UE_LOG(LogSKGameplayMessageSubsystem, Warning, TEXT("Tried to register listener with invalid channel."));
		return FSKGameplayMessageListenerHandle();
	}

	FChannelListenerList& ChannelList = ListenerMap.FindOrAdd(Channel);

	FSKGameplayMessageListenerData NewListener;
	NewListener.ReceivedCallback = MoveTemp(Callback);
	NewListener.HandleID = ++ChannelList.HandleID;
	NewListener.MatchType = MatchType;
	NewListener.ListenerStructType = StructType;
	NewListener.bHadValidType = (StructType != nullptr);

	ChannelList.Listeners.Add(MoveTemp(NewListener));

	UE_LOG(LogSKGameplayMessageSubsystem, Log, TEXT("Registered listener on %s (HandleID: %d)"),
		*Channel.ToString(), ChannelList.HandleID);

	return FSKGameplayMessageListenerHandle(this, Channel, ChannelList.HandleID);
}

void USKGameplayMessageSubsystem::UnregisterListenerInternal(FGameplayTag Channel, int32 HandleID)
{
	if (FChannelListenerList* ChannelList = ListenerMap.Find(Channel))
	{
		int32 RemovedCount = ChannelList->Listeners.RemoveAll(
			[HandleID](const FSKGameplayMessageListenerData& Data)
			{
				return Data.HandleID == HandleID;
			});

		if (RemovedCount > 0)
		{
			UE_LOG(LogSKGameplayMessageSubsystem, Log,
				TEXT("Unregistered listener from %s (HandleID: %d)"),
				*Channel.ToString(), HandleID);
		}

		if (ChannelList->Listeners.IsEmpty())
		{
			ListenerMap.Remove(Channel);
		}
	}
}

void USKGameplayMessageSubsystem::Deinitialize()
{
	ListenerMap.Empty();
	UE_LOG(LogSKGameplayMessageSubsystem, Log, TEXT("SKGameplayMessageSubsystem Deinitialized."));
}
