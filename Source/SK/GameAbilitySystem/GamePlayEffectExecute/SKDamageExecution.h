// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "SKDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USKDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	USKDamageExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	//ASC 비참조 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Damage")
	float NonASCAttackPower = 0.f;

	//데미지계수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Damage")
	float GEDamageCoefficient = 1.f;

	//계산용 attribute
	FGameplayEffectAttributeCaptureDefinition AttackDef;
	FGameplayEffectAttributeCaptureDefinition ArmorDef;
};
