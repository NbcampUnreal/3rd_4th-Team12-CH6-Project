// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterDamageWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class SK_API UMonsterDamageWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SettingWidget(float Damage, const FVector2D RendPos, int32 AttackType);
	void Deactivate();
	
	bool IsActive() const {return bIsActive;}
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY()
	bool bIsActive = false;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageText;
	
	FTimerHandle HideTimerHandle;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_TextMove;
};
