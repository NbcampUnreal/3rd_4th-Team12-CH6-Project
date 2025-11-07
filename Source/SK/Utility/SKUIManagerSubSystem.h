// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SKGameplayMessageSubsystem.h"
#include "SKGameplayMessageTypes.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "SKUIManagerSubSystem.generated.h"

class UAbilitySystemComponent;
class UCommonActivatableWidget;
class UUILayoutDataAsset;

/**
 * 
 */
UCLASS()
class SK_API USKUIManagerSubSystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:


	UFUNCTION(BlueprintCallable, Category = "UI")
	void SettingLayout();

	UFUNCTION(BlueprintCallable, Category="UI")
	void RemoveLayout();
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category="UI")
	void CreateLayoutWidget();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetLayoutVisibeByTag(FGameplayTag LayoutTag);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetLayoutHiddenByTag(FGameplayTag LayoutTag);

	UPROPERTY()
	UUILayoutDataAsset* SwtichAbleLayoutData;

	UPROPERTY()
	UUILayoutDataAsset* ConfirmLayoutData;
	
	UFUNCTION()
	void CreateLayoutFromData(UUILayoutDataAsset* CreateData, APlayerController* OwningPC);

private:
	/** 메시지 핸들 (등록 해제용) */
	FSKGameplayMessageListenerHandle LayoutSwitchHandle;

	FSKGameplayMessageListenerHandle RequestConfirmHandle;

	FSKGameplayMessageListenerHandle ConfirmResponseHandle;

	/** 메시지를 수신했을 때 실행될 함수 */
	void OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message);

	void OnRequestConfirmMessageReceived(FGameplayTag Channel, const FConfirmUIMessage& Message);

	void OnConfirmResponseMessageReceived(FGameplayTag Channel, const FConfirmResponseMessage& Message);
	
	UPROPERTY()
	TMap<FGameplayTag, UCommonActivatableWidget*> LayoutWidgets;

	UPROPERTY()
	TMap<FGameplayTag, UCommonActivatableWidget*> ConfirmLayoutWidgets;

	UPROPERTY()
	FGameplayTag CurrentLayoutTag;
};
