// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/DynamicEntryBoxBase.h"
#include "SKSlotBox.generated.h"

class USKLayoutWidgetBase;
/**
 * 
 */
UCLASS()
class SK_API USKSlotBox : public UDynamicEntryBoxBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "SlotBox")
	UUserWidget* GetCurrentSlotWidgetFromBox() {return CurrentSlotWidget;}

	// 이 SlotBox가 담당하는 슬롯 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slot")
	FGameplayTag BoxSlotTag;
protected:
	virtual void SynchronizeProperties() override;
 
	// 부모 LayoutWidget 참조 (알림 받기 용)
	UPROPERTY(BlueprintReadWrite, Category="Slot")
	USKLayoutWidgetBase* ParentLayoutWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot")
	UUserWidget* CurrentSlotWidget;
	
	// 부모와 연결 및 델리게이트 바인딩
	UFUNCTION(BlueprintCallable, Category="Slot")
	void InitializeSlotBox(UUserWidget* InParent);
	
	// 슬롯 데이터 변경 콜백
	UFUNCTION()
	void OnSlotDataChanged();
 
	// 슬롯 새로고침
	void SettingSlot();

	TSharedRef<SWidget> RebuildWidget() override;
};
