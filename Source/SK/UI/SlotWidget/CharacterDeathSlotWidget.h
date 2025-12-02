// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterDeathSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class SK_API UCharacterDeathSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	// 위젯이 화면에 나타날 때 실행되는 함수
	UFUNCTION(BlueprintCallable)
	void PlayDeathSequence();
	
protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* DeathAnim;

	// 애니메이션 끝났을 때 처리
	UFUNCTION()
	void OnDeathAnimationFinished();
};
