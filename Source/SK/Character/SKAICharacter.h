// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SKCharacterBase.h"
#include "SKAICharacter.generated.h"

class UWidgetComponent;
class UMonsterHealthWidget;
class UMonsterDamageWidget;
/**
 * 
 */
UCLASS()
class SK_API ASKAICharacter : public ASKCharacterBase
{
	GENERATED_BODY()
public:
	ASKAICharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	UWidgetComponent* HPBarWidgetComponent;

	UPROPERTY()
	UMonsterHealthWidget* HPBarWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	UWidgetComponent* DamageTextWidgetComponent;

	UPROPERTY()
	UMonsterDamageWidget* DamageTextWidget;
};
