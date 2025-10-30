// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "UILayoutDataAsset.generated.h"

class UCommonActivatableWidget;

/**
 * 레이아웃(메인 위젯 정보)
 */
USTRUCT(BlueprintType)
struct FLayoutData
{
	GENERATED_BODY()

public:
	// 레이아웃으로 사용할 CommonActivatableWidget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layout")
	TSubclassOf<UCommonActivatableWidget> LayoutWidgetClass;

	// 레이아웃 식별 태그 (예: Layout.MainHUD)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layout")
	FGameplayTag LayoutTag;
};

/**
 * 슬롯(개별 위젯 정보)
 */
USTRUCT(BlueprintType)
struct FSlotWidgetData
{
	GENERATED_BODY()

public:
	// 슬롯에 들어갈 위젯
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slot")
	TSubclassOf<UCommonActivatableWidget> WidgetClass;

	// 슬롯 식별용 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slot")
	FGameplayTag SlotTag;
};

/**
 * 
 */
UCLASS()
class SK_API UUILayoutDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// 단일 레이아웃
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Layout")
	FLayoutData Layout;

	// 슬롯 배열
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slot")
	TArray<FSlotWidgetData> Slots;	
};
