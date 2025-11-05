// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SKNativeGameplayTags.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "SKUIManagerSubSystem.generated.h"

class UAbilitySystemComponent;
class UCommonActivatableWidget;
class UUILayoutDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConfirmResult, bool, bResult);
/**
 * 
 */
UCLASS()
class SK_API USKUIManagerSubSystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="UI")
	void CreateLayoutWidget();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetLayoutVisibeByTag(FGameplayTag LayoutTag);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetLayoutHiddenByTag(FGameplayTag LayoutTag);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SettingLayout();

	UFUNCTION(BlueprintCallable, Category = "UI")
	UAbilitySystemComponent* GetCachedASC() { return CachedASC; }
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void RequestConfirmUI(FDataTableRowHandle ConfirmUIDataRow);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void RequestResult(bool bResult);

	UPROPERTY(BlueprintAssignable, Category = "UI|Confirm")
	FOnConfirmResult OnConfirmResult;
	
	//Debug용 함수 추후 사용 안할 시 제거
	UFUNCTION(BlueprintCallable, Category = "UI")
	FGameplayTagContainer GetLayoutTags();

	UFUNCTION(BlueprintCallable, Category = "UI")
	FGameplayTag GetCurrentLayoutTag() {return CurrentLayoutTag;}
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category="UI")
	void RemoveLayout();

	UPROPERTY()
	UUILayoutDataAsset* SwtichAbleLayoutData;

	UPROPERTY()
	UUILayoutDataAsset* ConfirmLayoutData;
	
	UFUNCTION()
	void CreateLayoutFromData(UUILayoutDataAsset* CreateData, APlayerController* OwningPC);

	UFUNCTION(BlueprintCallable, Category="UI")
	void HandleSwitchLayout(const FGameplayTag Tag, int32 NewCount);

	UFUNCTION(BlueprintCallable, Category="UI")
	void AddSwitchLayoutRegisterEvent(FGameplayTag RegisterTag);
private:
	UPROPERTY()
	TMap<FGameplayTag, UCommonActivatableWidget*> LayoutWidgets;

	TMap<FGameplayTag, FDelegateHandle> LayoutTagDelegateHandles;

	UPROPERTY()
	TMap<FGameplayTag, UCommonActivatableWidget*> ConfirmLayoutWidgets;
	
	UPROPERTY()
	FGameplayTagContainer SwitchableLayoutTags;
	
	UPROPERTY()
	UAbilitySystemComponent* CachedASC;

	UPROPERTY()
	FGameplayTag CurrentLayoutTag;
};
