// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Templates/Function.h"
#include "SKGameplayMessageTypes.generated.h"
enum class EEquipmentSlotType : uint8;
enum class EInventoryItemType : uint8;
class USKInventoryItemData;
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

USTRUCT(BlueprintType)
struct FSlotVisibilityMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag LayoutTag;

	/** 여러 슬롯을 한 번에 제어할 수 있음 */
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer SlotTags;

	/** true면 표시, false면 숨김 */
	UPROPERTY(BlueprintReadWrite)
	bool bVisible = true;

	/** 표시 지속 시간 (0이면 무제한 표시) */
	UPROPERTY(BlueprintReadWrite)
	float VisibleDuration = 0.0f;
};

USTRUCT(BlueprintType)
struct FToolTipSwitch
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 MessageItemID;

	UPROPERTY(BlueprintReadWrite)
	bool bEnter;
};

USTRUCT(BlueprintType)
struct FItemSwitchMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EInventoryItemType ItemType;

	UPROPERTY(BlueprintReadWrite)
	EEquipmentSlotType EquipmentType;

	UPROPERTY(BlueprintReadWrite)
	int32 QuickSlotNumber;
};

USTRUCT(BlueprintType)
struct FItemAddMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 ItemID;

	UPROPERTY(BlueprintReadWrite)
	int32 ItemCount;
};

USTRUCT(BlueprintType)
struct FQuickSlotCooldown
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 SlotIndex;

	UPROPERTY(BlueprintReadWrite)
	float Cooldown;
};

USTRUCT(BlueprintType)
struct FSettingBossHPWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	APawn* OwnerActor;

	UPROPERTY(BlueprintReadWrite)
	FText NameText;

	/* 필요하면 추가. 보스 이미지
	UPROPERTY(BlueprintReadWrite)
	UImage* WidgetImage
	*/
};

USTRUCT(BlueprintType)
struct FLoadingUIVisible
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool bVisible;
};

UENUM(BlueprintType)
enum class EUIMessageType : uint8
{
	BonfireMove,
	Inventory,
	Confirm,
	Equipmentmain,
	EquipmentSelect,
};

USTRUCT(BlueprintType)
struct FUIInteractionMoveMessage
{
	GENERATED_BODY()

	UPROPERTY()
	EUIMessageType Type;
	
	UPROPERTY(BlueprintReadWrite)
	int32 MoveDirection;
};

USTRUCT(BlueprintType)
struct FSkillUIMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 SkillNum;

	UPROPERTY(BlueprintReadWrite)
	bool bSuccess;
};

USTRUCT(BlueprintType)
struct FDialogSendMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString MessageText;
};

USTRUCT(BlueprintType)
struct FDialogSkipMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 TempNum;
};