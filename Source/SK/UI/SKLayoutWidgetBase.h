// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameData/UILayoutDataAsset.h"
#include "SKLayoutWidgetBase.generated.h"

class USKSlotBox;
class UAbilitySystemComponent;
// 슬롯 데이터 변경 알림 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlotDataChanged);

USTRUCT(BlueprintType)
struct FSlotEventBinding
{
	GENERATED_BODY()

	// 해당 슬롯 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SlotEvent")
	FGameplayTag TargetSlotTag;

	// 이 태그의 변경 이벤트를 감지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SlotEvent")
	FGameplayTag EventTag;
};

/**
 * 레이아웃 위젯 베이스 클래스
 * - FSlotWidgetData 배열로 슬롯 데이터 저장 및 관리
 * - 슬롯 데이터 변경 시 알림 이벤트 브로드캐스트 제공
 */
UCLASS()
class SK_API USKLayoutWidgetBase : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	USKLayoutWidgetBase();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	// 슬롯 데이터 변경 시 블루프린트에서 바인딩 가능한 이벤트
	UPROPERTY(BlueprintAssignable, Category="Event")
	FOnSlotDataChanged OnSlotDataChanged;
 
	// 슬롯 데이터 Set 함수
	UFUNCTION(BlueprintCallable, Category="Slot")
	void SetSlotData(const TArray<FSlotWidgetData>& NewSlots);
 
	// 슬롯 데이터 Get 함수
	UFUNCTION(BlueprintCallable, Category="Slot")
	TArray<FSlotWidgetData> GetSlotData();
	
	// ★ InputConfig만 적용하고 InputMode는 유지하는 오버라이드 추가
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	UFUNCTION(BlueprintCallable, Category="Slot")
	void ChangeVisibleSlotByTag(FGameplayTag ChangeSlotTag, bool bvisible);

	UFUNCTION()
	void OnGameplayTagChanged(const FGameplayTag Tag, int32 NewCount);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slot")
	TArray<FSlotWidgetData> SlotWidgetDataArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActiveInput")
	FUIInputConfig ActivaeInputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SlotEvent")
	TArray<FSlotEventBinding> SlotEventBindings;

	TMap<FGameplayTag, TArray<FDelegateHandle>> ASCEventHandles;
	
	UPROPERTY()
	UAbilitySystemComponent* CachedASC = nullptr;
	
	// 슬롯 데이터 변경 알림 함수(내부 호출용)
	void NotifySlotDataChanged() const;

	UFUNCTION(BlueprintCallable, Category="Slot")
	USKSlotBox* FindDynamicEntryBoxBySlotTag(const FGameplayTag& InSlotTag) const;

	UFUNCTION(BlueprintCallable, Category="Slot")
	void UnregisterTagEvent();
};
