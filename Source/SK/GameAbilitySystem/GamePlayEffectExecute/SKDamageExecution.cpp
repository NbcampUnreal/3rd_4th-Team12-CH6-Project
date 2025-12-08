// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/GamePlayEffectExecute/SKDamageExecution.h"

#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "GameAbilitySystem/Attribute/AI/SKAIAttributeSet.h"
#include "GameData/SKGameConstant.h"



USKDamageExecution::USKDamageExecution()
{
	AttackDef = FGameplayEffectAttributeCaptureDefinition(
	  USKAttributeSet::GetAttackAttribute(),
	  EGameplayEffectAttributeCaptureSource::Source,
	  true
  );

	ArmorDef = FGameplayEffectAttributeCaptureDefinition(
		USKAIAttributeSet::GetArmorAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		true
	);
	
	RelevantAttributesToCapture.Add(AttackDef);
	RelevantAttributesToCapture.Add(ArmorDef);
}

void USKDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	//GE가 적용될 스펙
	const  FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// 공격자/피격자 ASC
	UAbilitySystemComponent* SourceASC = Spec.GetContext().GetOriginalInstigatorAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	float AttackPower = 0.f;
	float ArmorPower = 0.f;
	
	//계산 조건 파라미터
	FAggregatorEvaluateParameters EvaluateParams;


	if (SourceASC)
	{
		if (const USKAttributeSet* SourceSet = SourceASC->GetSet<USKAttributeSet>())
		{
			AttackPower = SourceSet->GetAttack();
		}
	}

	if (TargetASC)
	{
		if (const USKAIAttributeSet* TargetSet = TargetASC->GetSet<USKAIAttributeSet>())
		{
			ArmorPower = TargetSet->GetArmor();
		}
	}
	
	if (NonASCAttackPower > 0.f)
	{
		AttackPower = NonASCAttackPower;
	}
	

	// 기본값 보정
	AttackPower = FMath::Max(AttackPower, 0.f);
	ArmorPower = FMath::Clamp(ArmorPower, 0.f, SKConstant::MaxArmorValue);

	// 데미지 감소율
	const float DamageMultiplier = 1.f - (ArmorPower / (ArmorPower + SKConstant::ArmorDamageDeclineRate));
	
	const float FinalDamage = AttackPower * DamageMultiplier * GEDamageCoefficient;
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		USKAIAttributeSet::GetHealthAttribute(),
		EGameplayModOp::Additive,
		-FinalDamage));
	
	UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), FinalDamage);
	
}
