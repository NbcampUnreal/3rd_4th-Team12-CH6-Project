// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/GamePlayEffectExecute/SKAIDamageExecution.h"

#include "GameAbilitySystem/Attribute/AI/SKAIAttributeSet.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "GameData/SKGameConstant.h"
#include "Utility/SKNativeGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Utility/HitReactUtils.h"
#include "Character/SKPlayerCharacter.h"


bool USKAIDamageExecution::IsFrontGuardable(
	const AActor* Attacker,
	const AActor* Defender,
	float DotThreshold
) const
{
	if (!Attacker || !Defender)
	{
		return false;
	}

	const FVector DefenderForward = Defender->GetActorForwardVector();
	const FVector AttackDir =
		(Attacker->GetActorLocation() - Defender->GetActorLocation()).GetSafeNormal();

	const float Dot = FVector::DotProduct(DefenderForward, AttackDir);

	return Dot >= DotThreshold;
}

USKAIDamageExecution::USKAIDamageExecution()
{
	AttackDef = FGameplayEffectAttributeCaptureDefinition(
	  USKAIAttributeSet::GetAttackAttribute(),
	  EGameplayEffectAttributeCaptureSource::Source,
	  true
  );

	ArmorDef = FGameplayEffectAttributeCaptureDefinition(
		USKAttributeSet::GetArmorAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		true
	);
	
	RelevantAttributesToCapture.Add(AttackDef);
	RelevantAttributesToCapture.Add(ArmorDef);
}

void USKAIDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
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
		if (const USKAIAttributeSet* SourceSet = SourceASC->GetSet<USKAIAttributeSet>())
		{
			AttackPower = SourceSet->GetAttack();
		}
	}

	if (TargetASC)
	{
		if (const USKAttributeSet* TargetSet = TargetASC->GetSet<USKAttributeSet>())
		{
			ArmorPower = TargetSet->GetArmor();
		}
	}
	
	if (NonASCAttackPower > 0.f)
	{
		AttackPower = NonASCAttackPower;
	}
	
	//기본값 보정
	AttackPower = FMath::Max(AttackPower,0.f);
	ArmorPower = FMath::Clamp(ArmorPower,0.f,SKConstant::MaxArmorValue);
	
	const float DamageMultiplier = 1.f - (ArmorPower / (ArmorPower + SKConstant::ArmorDamageDeclineRate));
	float FinalDamage  = AttackPower* DamageMultiplier;

	// ==============================
	// 1. 플레이어 무적 판정
	// ==============================
	const bool bIsInvincibility =
		TargetASC->HasMatchingGameplayTag(TAG_State_invincibility);
	if (bIsInvincibility)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is invincibility"));
		return;
	}
	

	// ==============================
	// 2. 가드 / 퍼펙트 가드 판정
	// ==============================

	const bool bIsGuarding =
		TargetASC->HasMatchingGameplayTag(TAG_State_Action_Guard);

	const bool bIsPerfectGuard =
		TargetASC->HasMatchingGameplayTag(TAG_State_Action_Guard_Perfect);

	AActor* SourceActor = SourceASC->GetAvatarActor();
	AActor* TargetActor = TargetASC->GetAvatarActor();
	const bool bIsGuardRotator = IsFrontGuardable(SourceActor, TargetActor, GuardAngle);

	float StaminaCost = 0.f;

	// 퍼펙트 가드 → 데미지 0
	if (bIsPerfectGuard && bIsGuarding && bIsGuardRotator)
	{
		FinalDamage = 0.f;
		StaminaCost = PerfectGuardCost;
	}
	// 일반 가드 → 데미지 감소
	else if (bIsGuarding && bIsGuardRotator)
	{
		FinalDamage *= DamageReducedByGuard;

		//공격 타입 분기
		if (Spec.DynamicGrantedTags.HasTag(TAG_Attack_Heavy))
		{
			StaminaCost = HeavyAttackGuardCost;
		}
		else
		{
			StaminaCost = NormalAttackGuardCost;
		}
	}
	//방어 실패로 히트 리액션 함수 호출
	else
	{
		HandleHitReaction(Spec, TargetASC, TargetActor);
	}

	// ==============================
	// 3. Guard 성공 이벤트 전송
	// ==============================
	if ((bIsGuarding || bIsPerfectGuard) && bIsGuardRotator && FinalDamage >= 0.f)
	{
		//AActor* TargetActor = TargetASC->GetAvatarActor();
		if (TargetActor)
		{
			FGameplayEventData EventData;
			EventData.EventTag = TAG_Event_Guard_Success;
			EventData.Target = TargetActor;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				TargetActor,
				TAG_Event_Guard_Success,
				EventData
			);
		}
	}

	if (FinalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
				USKAttributeSet::GetHealthAttribute(),
				EGameplayModOp::Additive,
				-FinalDamage));
	}

	if (StaminaCost > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
				USKAttributeSet::GetStaminaAttribute(),
				EGameplayModOp::Additive,
				-StaminaCost));
	}
}

void USKAIDamageExecution::HandleHitReaction(const FGameplayEffectSpec& Spec, UAbilitySystemComponent* TargetASC,
	AActor* TargetActor) const
{
	if (!TargetASC || !TargetActor)
		return;

	// 공격자 (AI)
	AActor* InstigatorActor = Spec.GetContext().GetOriginalInstigator();
	if (!InstigatorActor)
		return;

	// Player만 HitReact (필요시 조건 조절)
	if (!TargetActor->IsA(ASKPlayerCharacter::StaticClass()))
		return;

	const FGameplayTag HitStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Condition.Hit"));

	if (TargetASC->HasMatchingGameplayTag(HitStateTag))
	{
		return;
	}

	// 방향 판정
	const EHitReactAnim ReactType =
		DetermineHitReactAnim(
			InstigatorActor->GetActorLocation(),
			TargetActor
		);

	// 5️⃣ Hit 상태 부여 (Lock)
	if (HitConditionEffect)
	{
		const UGameplayEffect* HitGE = HitConditionEffect->GetDefaultObject<UGameplayEffect>();
		
		TargetASC->ApplyGameplayEffectToSelf(
			HitGE,
			1.f,
			TargetASC->MakeEffectContext()
		);
	}

	// GameplayCue 선택
	FGameplayTag CueTag;

	switch (ReactType)
	{
	case EHitReactAnim::FrontLeft:
		CueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.HitReact.FrontLeft"));
		break;

	case EHitReactAnim::FrontRight:
		CueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.HitReact.FrontRight"));
		break;

	case EHitReactAnim::Back:
		CueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.HitReact.Back"));
		break;

	default:
		return;
	}

	// 서버에서 GameplayCue 실행
	TargetASC->ExecuteGameplayCue(CueTag);
}
