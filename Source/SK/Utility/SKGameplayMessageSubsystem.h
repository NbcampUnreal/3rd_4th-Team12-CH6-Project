// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SKGameplayMessageSubsystem.generated.h"

class USKGameplayMessageSubsystem;

/** 메시지 리스너를 구분하기 위한 핸들 */
USTRUCT(BlueprintType)
struct FSKGameplayMessageListenerHandle
{
	GENERATED_BODY()

	FSKGameplayMessageListenerHandle() {}

	void Unregister();

	bool IsValid() const { return ID != 0; }

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<USKGameplayMessageSubsystem> Subsystem;

	UPROPERTY(Transient)
	FGameplayTag Channel;

	UPROPERTY(Transient)
	int32 ID = 0;

	friend USKGameplayMessageSubsystem;

	FSKGameplayMessageListenerHandle(USKGameplayMessageSubsystem* InSubsystem, FGameplayTag InChannel, int32 InID)
		: Subsystem(InSubsystem), Channel(InChannel), ID(InID)
	{}
};

UENUM(BlueprintType)
enum class ESKGameplayMessageMatch : uint8
{
	// 정확히 일치하는 채널만 수신
	ExactMatch,

	// 해당 채널을 루트로 하는 모든 메시지를 수신
	PartialMatch
};

/** 리스너 데이터 */
USTRUCT()
struct FSKGameplayMessageListenerData
{
	GENERATED_BODY()

	TFunction<void(FGameplayTag, const UScriptStruct*, const void*)> ReceivedCallback;

	int32 HandleID;
	ESKGameplayMessageMatch MatchType;
	TWeakObjectPtr<const UScriptStruct> ListenerStructType = nullptr;
	bool bHadValidType = false;
};



/**
 * 메시지 송수신을 관리하는 서브시스템
 * ASC나 Actor 참조 없이 메시지 기반의 통신 가능
 */
UCLASS()
class SK_API USKGameplayMessageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	/** 현재 월드 컨텍스트에서 메시지 서브시스템 가져오기 */
	static USKGameplayMessageSubsystem* Get(const UObject* WorldContextObject)
	{
		if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				return GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
			}
		}
		return nullptr;
	}

	// -------------------------
	// 메시지 브로드캐스트
	// -------------------------

	template <typename FMessageStructType>
	void BroadcastMessage(FGameplayTag Channel, const FMessageStructType& Message)
	{
		const UScriptStruct* StructType = ::TBaseStructure<FMessageStructType>::Get();
		BroadcastMessageInternal(Channel, StructType, &Message);
	}

	// -------------------------
	// 리스너 등록 (Lambda)
	// -------------------------

	template <typename FMessageStructType>
	FSKGameplayMessageListenerHandle RegisterListener(
		FGameplayTag Channel,
		TFunction<void(FGameplayTag, const FMessageStructType&)>&& Callback,
		ESKGameplayMessageMatch MatchType = ESKGameplayMessageMatch::ExactMatch)
	{
		auto ThunkCallback = [InnerCallback = MoveTemp(Callback)]
		(FGameplayTag ActualTag, const UScriptStruct* SenderStructType, const void* SenderPayload)
		{
			InnerCallback(ActualTag, *reinterpret_cast<const FMessageStructType*>(SenderPayload));
		};

		const UScriptStruct* StructType = TBaseStructure<FMessageStructType>::Get();
		return RegisterListenerInternal(Channel, ThunkCallback, StructType, MatchType);
	}

	// -------------------------
	// 리스너 등록 (객체 멤버 함수)
	// -------------------------

	template <typename FMessageStructType, typename TOwner = UObject>
	FSKGameplayMessageListenerHandle RegisterListener(
		FGameplayTag Channel,
		TOwner* Object,
		void(TOwner::* Function)(FGameplayTag, const FMessageStructType&))
	{
		TWeakObjectPtr<TOwner> WeakObject(Object);

		return RegisterListener<FMessageStructType>(
			Channel,
			[WeakObject, Function](FGameplayTag Tag, const FMessageStructType& Payload)
			{
				if (TOwner* StrongObject = WeakObject.Get())
				{
					(StrongObject->*Function)(Tag, Payload);
				}
			});
	}

	// -------------------------
	// 리스너 해제
	// -------------------------

	void UnregisterListener(FSKGameplayMessageListenerHandle Handle)
	{
		if (!Handle.IsValid())
			return;

		UnregisterListenerInternal(Handle.Channel, Handle.ID);
	}
	
protected:
	void BroadcastMessageInternal(FGameplayTag Channel, const UScriptStruct* StructType, const void* MessageBytes);
	FSKGameplayMessageListenerHandle RegisterListenerInternal(
		FGameplayTag Channel,
		TFunction<void(FGameplayTag, const UScriptStruct*, const void*)>&& Callback,
		const UScriptStruct* StructType,
		ESKGameplayMessageMatch MatchType);
	void UnregisterListenerInternal(FGameplayTag Channel, int32 HandleID);
	
	virtual void Deinitialize() override;
private:
	struct FChannelListenerList
	{
		TArray<FSKGameplayMessageListenerData> Listeners;
		int32 HandleID = 0;
	};

	TMap<FGameplayTag, FChannelListenerList> ListenerMap;
};

