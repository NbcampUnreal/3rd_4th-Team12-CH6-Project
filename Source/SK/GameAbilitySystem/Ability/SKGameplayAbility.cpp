// Fill out your copyright notice in the Description page of Project Settings.



#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "Character/SKPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "Controller/SKPlayerController.h"

USKGameplayAbility::USKGameplayAbility()
{
}

void USKGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USKGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USKGameplayAbility::ApplyDamageFromTrace()
{
	
}

void USKGameplayAbility::OnStopAttackTrace_Server()
{
	//몽타주 1번에 여러 공격이 들어갈 때 데미지 빨리 처리 시 불리는 함수
	ApplyDamageFromTrace();
}

void USKGameplayAbility::AddAttackTypeToEffectSpec(FGameplayEffectSpec& Spec) const
{
	if (AttackTypeTag.IsValid())
	{
		Spec.DynamicGrantedTags.AddTag(AttackTypeTag);
	}
}

void USKGameplayAbility::ApplyMotionWarp(ASKPlayerCharacter* SKPlayer)
{
	UMotionWarpingComponent* MW = SKPlayer->GetMotionWarpingComponent();
	if (!MW) return;

	FVector TargetLoc = GetAttackTargetLocation();

	if (TargetLoc !=  FVector::ZeroVector)
	{
		MW->AddOrUpdateWarpTargetFromLocation(
			TEXT("Target"),
			TargetLoc
		);
		
	}

}

FVector USKGameplayAbility::GetAttackTargetLocation() const
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	ASKPlayerCharacter* PC = Cast<ASKPlayerCharacter>(Avatar);
	if (!PC)
		return FVector::ZeroVector;

	ASKPlayerController* SKPC =
		Cast<ASKPlayerController>(PC->GetController());

	if (SKPC)
	{
		if (AActor* LockedTarget = SKPC->GetLockedTarget())
		{
			FVector TargetLoc = LockedTarget->GetActorLocation();

			FVector ToPlayer =
				(PC->GetActorLocation() - TargetLoc).GetSafeNormal();

			// 적 앞에서 멈추기
			TargetLoc += ToPlayer * 80.f;
		}
	}

	return FVector::ZeroVector;
	// USkeletalMeshComponent* MeshComp = PC->GetMesh();
	// if (!MeshComp)
	// {
	// 	return PC->GetActorLocation();
	// }
	//
	// FVector Forward = MeshComp->GetForwardVector();
	// Forward.Z = 0.f;                 // 지면 기준
	// Forward.Normalize();
	//
	// return PC->GetActorLocation() + Forward * 100.f;
	
	//
	// FRotator ControlRot = PC->GetControlRotation();
	// FRotator YawOnly(0.f, ControlRot.Yaw, 0.f);
	// FVector Forward = YawOnly.Vector();
	//
	// return PC->GetActorLocation() + Forward * 100.f;
}

void USKGameplayAbility::ApplyHeatGE(int32 HeatIndex,UAbilitySystemComponent* SourceASC)
{
	// TSubclassOf<UGameplayEffect> HeatEffectClass =
	// 	HeatGE[HeatIndex];
	TSubclassOf<UGameplayEffect> HeatEffectClass =
		HeatGE[0];

	if (HeatEffectClass)
	{
		FGameplayEffectContextHandle HeatContext =
			SourceASC->MakeEffectContext();

		HeatContext.AddSourceObject(this);

		FGameplayEffectSpecHandle HeatSpecHandle =
			SourceASC->MakeOutgoingSpec(HeatEffectClass, 1.f, HeatContext);

		if (HeatSpecHandle.IsValid())
		{
			SourceASC->ApplyGameplayEffectSpecToSelf(
				*HeatSpecHandle.Data.Get()
			);
		}
	}
}




