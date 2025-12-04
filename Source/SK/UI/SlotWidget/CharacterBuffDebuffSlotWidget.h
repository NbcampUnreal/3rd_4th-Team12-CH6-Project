// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "GameData/StaticData/BuffDebuffIconDataTable.h"
#include "CharacterBuffDebuffSlotWidget.generated.h"

class UUniformGridPanel;
class UBuffDebuffIconWidget;
struct FModifiedAttributeArray;
struct FGameplayTagContainer;
struct FModifiedAttributeInfo;
class ASKPlayerState;
/**
 * 
 */
UCLASS()
class SK_API UCharacterBuffDebuffSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void TryBind();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ASKPlayerState* CachedPS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Buff")
	TSubclassOf<UBuffDebuffIconWidget> BuffIconClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Buff")
	int32 MaxPoolSize = 8;

	UPROPERTY()
	TArray<UBuffDebuffIconWidget*> WidgetPool;
	
	TMap<FActiveGameplayEffectHandle, TArray<UBuffDebuffIconWidget*>> ActiveWidgets;

	UBuffDebuffIconWidget* GetWidgetFromPool();
	
	void ReturnWidgetToPool(UBuffDebuffIconWidget* Widget);

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* BuffContainer;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* DebuffContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxColumn = 5;

	UFUNCTION()
	const FBuffDebuffIconDataTable& GetIconDataTable(FModifiedAttributeInfo Info);
	
	UFUNCTION()
	bool IsBuff(const FModifiedAttributeInfo& Info);

	UFUNCTION()
	void AddWidgetToContainer(UBuffDebuffIconWidget* Widget, bool bIsBuff);

	UFUNCTION()
	void RemoveWidgetFromContainer(UBuffDebuffIconWidget* Widget, bool bIsBuff);

	UFUNCTION()
	void RemoveWidgetsFromHandle(FActiveGameplayEffectHandle EffectHandle);
	
	UFUNCTION()
	void OnBuffAddedHandler(FActiveGameplayEffectHandle AddEffectHandle, const FModifiedAttributeArray& AddModifiedAttributes, FGameplayTagContainer AddBuffTags, float Duration);
	
	UFUNCTION()
	void OnBuffRemovedHandler(FActiveGameplayEffectHandle EffectHandle, FModifiedAttributeArray AddModifiedAttributes);
	
	UFUNCTION()
	void OnBuffTimeChangedHandler(FActiveGameplayEffectHandle EffectHandle, float NewStartTime, float NewDuration);

	UFUNCTION()
	void OnBuffStackChangedHandler(FActiveGameplayEffectHandle EffectHandle, int32 NewStack, int32 OldStack);
};
