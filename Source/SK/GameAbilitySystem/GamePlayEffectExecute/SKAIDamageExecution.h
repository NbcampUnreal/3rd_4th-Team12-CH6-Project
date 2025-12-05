// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "SKAIDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USKAIDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	USKAIDamageExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	//ASC 비참조 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Damage")
	float NonASCAttackPower = 0.f;


	//계산용 attribute
	FGameplayEffectAttributeCaptureDefinition AttackDef;
	FGameplayEffectAttributeCaptureDefinition ArmorDef;
};
