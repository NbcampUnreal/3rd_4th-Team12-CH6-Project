// Fill out your copyright notice in the Description page of Project Settings.


#include "GameAbilitySystem/Ability/HitReact/GA_HitReact.h"
#include "AbilitySystemComponent.h"
#include "Utility/HitReactUtils.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Character/SKPlayerCharacter.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_HitReact::UGA_HitReact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	AbilityTags.AddTag(TAG_Ability_HitReaction);
	ActivationBlockedTags.AddTag(TAG_State_Condition_Hit);
}

void UGA_HitReact::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UE_LOG(LogTemp, Warning, TEXT("GA_HitReact Start"));
    
	if (!TriggerEventData || !ActorInfo)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    UAbilitySystemComponent* TargetASC = ActorInfo->AbilitySystemComponent.Get();
    AActor* TargetActor = ActorInfo->AvatarActor.Get();

    if (!TargetASC || !TargetActor)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // Player만 HitReact
    if (!TargetActor->IsA(ASKPlayerCharacter::StaticClass()))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // 이미 히트 상태면 무시
    if (TargetASC->HasMatchingGameplayTag(TAG_State_Condition_Hit))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    /* =========================
     * HitType 판정
     * ========================= */

    EHitReactType HitType = EHitReactType::Normal;

    if (TriggerEventData->InstigatorTags.HasTag(TAG_Attack_Heavy))
    {
        HitType = EHitReactType::Heavy;
    }
    else if (TriggerEventData->InstigatorTags.HasTag(TAG_Attack_UnGuardable))
    {
        HitType = EHitReactType::Unblockable;
    }

    /* =========================
     * Hit 상태 부여
     * ========================= */

    if (HitConditionEffect)
    {
        TargetASC->ApplyGameplayEffectToSelf(
            HitConditionEffect->GetDefaultObject<UGameplayEffect>(),
            1.f,
            TargetASC->MakeEffectContext()
        );
    }

    /* =========================
     * GameplayCue 실행
     * ========================= */

    FGameplayTag CueTag;

    switch (HitType)
    {
    case EHitReactType::Normal:
        CueTag = TAG_GameplayCue_HitReact_Normal;
        break;
    case EHitReactType::Heavy:
        CueTag = TAG_GameplayCue_HitReact_Heavy;
        break;
    case EHitReactType::Unblockable:
        CueTag = TAG_GameplayCue_HitReact_Unblockable;
        break;
    }

    if (HitType == EHitReactType::Normal)
    {
        ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
        if (!Character) return;

        const AActor* InstigatorActor = TriggerEventData->Instigator.Get();

        if (!InstigatorActor) return;
        
        FVector LaunchDir = Character->GetActorLocation() - InstigatorActor->GetActorLocation();

        LaunchDir.Z = 0.f;
        LaunchDir = LaunchDir.GetSafeNormal();
        
        Character->LaunchCharacter(
            LaunchDir * PushStrength,
            true,   // XY Override
            false   // Z Override (뜰 필요 없음)
        );
    }

    TargetASC->ExecuteGameplayCue(
        CueTag,
        FGameplayCueParameters(TriggerEventData->ContextHandle)
    );

    EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UGA_HitReact::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
