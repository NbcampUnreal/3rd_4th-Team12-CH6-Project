// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterDamageContainerWidget.generated.h"

struct FOnAttributeChangeData;
class UMonsterDamageWidget;
class UCanvasPanel;
/**
 * 
 */
UCLASS()
class SK_API UMonsterDamageContainerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SettingWidget(APawn* OwnerPawn);
	
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* DamageCanvas;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMonsterDamageWidget> DamageTextEntryClass;

	UPROPERTY(EditDefaultsOnly)
	int32 PoolSize = 10;

	UPROPERTY()
	TArray<UMonsterDamageWidget*> DamageTextPool;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void AddDamageText(const FOnAttributeChangeData& Data);
};
