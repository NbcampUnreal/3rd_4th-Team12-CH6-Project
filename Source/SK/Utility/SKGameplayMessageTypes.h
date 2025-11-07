// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Templates/Function.h"
#include "SKGameplayMessageTypes.generated.h"
/**
 * 메시지 매칭 규칙
 */
UENUM(BlueprintType)
enum class EGameplayMessageMatch : uint8
{
	ExactMatch,
	PartialMatch
};

/**
 * 메시지 리스너 등록 시 전달되는 파라미터 구조체
 */
template<typename FMessageStructType>
struct FGameplayMessageListenerParams
{
	EGameplayMessageMatch MatchType = EGameplayMessageMatch::ExactMatch;

	TFunction<void(FGameplayTag, const FMessageStructType&)> OnMessageReceivedCallback;

	template<typename TOwner = UObject>
	void SetMessageReceivedCallback(TOwner* Object, void(TOwner::* Function)(FGameplayTag, const FMessageStructType&))
	{
		TWeakObjectPtr<TOwner> WeakObject(Object);
		OnMessageReceivedCallback = [WeakObject, Function](FGameplayTag Channel, const FMessageStructType& Payload)
		{
			if (TOwner* StrongObject = WeakObject.Get())
			{
				(StrongObject->*Function)(Channel, Payload);
			}
		};
	}
};

USTRUCT(BlueprintType)
struct FSwitchLayoutMessage
{
	GENERATED_BODY()

	/** 어떤 레이아웃 태그를 전환할지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag LayoutTag;

	/** 표시 여부 (true면 보이게, false면 숨김) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bVisible = true;

	FSwitchLayoutMessage() {}
	FSwitchLayoutMessage(FGameplayTag InTag, bool bInVisible)
		: LayoutTag(InTag), bVisible(bInVisible)
	{}
};

USTRUCT(BlueprintType)
struct FConfirmUIMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag SlotTag;

	UPROPERTY(BlueprintReadWrite)
	FText Title;

	UPROPERTY(BlueprintReadWrite)
	FText Message;

	UPROPERTY(BlueprintReadWrite)
	FText ConfirmText;

	UPROPERTY(BlueprintReadWrite)
	FText CancelText;
};

USTRUCT(BlueprintType)
struct FConfirmResponseMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag SlotTag;

	UPROPERTY(BlueprintReadWrite)
	bool bAccepted;
};