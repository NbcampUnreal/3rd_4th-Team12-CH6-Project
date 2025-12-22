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

	//가드로 인한 축소된 데미지 비율 //예)0.2는 데미지 80퍼 감소
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Damage")
	float DamageReducedByGuard = 0.2f;

	//일반공격 막았을 시 스테미너 소모 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Damage")
	float NormalAttackGuardCost = 10.0f;

	//강공격 막았을 시 스테미너 소모 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Damage")
	float HeavyAttackGuardCost = 25.0f;

	//퍼펙트 가드로 공격 막았을 시 스테미너 소모 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Damage")
	float PerfectGuardCost = 0.0f;

	bool IsFrontGuardable(const AActor* Attacker, const AActor* Defender, float DotThreshold) const;
	//방어 가능한 각도 -1 ~ 1  -1:후면,  1:정면,  0:측면
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Damage")
	float GuardAngle = 0.2f;
};
