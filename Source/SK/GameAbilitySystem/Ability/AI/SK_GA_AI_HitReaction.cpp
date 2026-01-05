#include "GameAbilitySystem/Ability/AI/SK_GA_AI_HitReaction.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Character/AI/SKAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

USK_GA_AI_HitReaction::USK_GA_AI_HitReaction()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.HitReaction")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.HitReaction")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.HitReaction")));
}

void USK_GA_AI_HitReaction::HitReaction(TObjectPtr<UAnimMontage> LocalAnimMontage)
{
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				LocalAnimMontage,
				1.0f,
				NAME_None,
				true,
				1.0f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_HitReaction::OnHitReactionCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_HitReaction::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_HitReaction::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_HitReaction::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_HitReaction::OnHitReactionCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_HitReaction::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	CommonEventTask->EndTask();
	/*
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	*/
	// 피격 방향 계산
	const FVector HitDirection = CalculateHitDirection(TriggerEventData);

	// 자연스러운 Knockback 시작
	StartSmoothKnockback(HitDirection);

	

	AnimMontage = GetAnimMontage("HitReaction");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	HitReaction(AnimMontage);
}

void USK_GA_AI_HitReaction::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (IsValid(OwnerASC))
	{
		OwnerASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.HitReaction")));
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


// ======================================================
// Smooth Knockback (Curve 없이)
// ======================================================

void USK_GA_AI_HitReaction::StartSmoothKnockback(const FVector& InDirection)
{
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar)
        return;

    ASKAICharacter* Character = Cast<ASKAICharacter>(Avatar);
    if (Character)
    {
        UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
        if (MoveComp && !MoveComp->IsMovingOnGround())
        {
            // 공중에서는 Knockback 적용 안 함
            return;
        }
    }

    KnockbackDirection = InDirection;
    KnockbackDirection.Z = 0.f;
    KnockbackDirection.Normalize();

    ElapsedTime = 0.f;
    PrevAlpha = 0.f;

    UWorld* World = GetWorld();
    if (!World)
        return;

    World->GetTimerManager().SetTimer(
        KnockbackTimerHandle,
        this,
        &USK_GA_AI_HitReaction::TickSmoothKnockback,
        0.016f, // 약 60fps
        true
    );
}

void USK_GA_AI_HitReaction::TickSmoothKnockback()
{
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar)
    {
        StopSmoothKnockback();
        return;
    }

    ElapsedTime += 0.016f;

    // 시간 비율 (0~1)
    float T = FMath::Clamp(
        ElapsedTime / KnockbackDuration,
        0.f,
        1.f
    );

    // Ease-Out (Quadratic)
    // Alpha = 1 - (1 - T)^2
    float Alpha = 1.f - FMath::Square(1.f - T);

    float DeltaAlpha = Alpha - PrevAlpha;
    PrevAlpha = Alpha;

    FVector Offset =
        KnockbackDirection * (KnockbackDistance * DeltaAlpha);

    FHitResult Hit;
    Avatar->AddActorWorldOffset(Offset, true, &Hit);

    if (Hit.bBlockingHit || T >= 1.f)
    {
        StopSmoothKnockback();
    }
}

void USK_GA_AI_HitReaction::StopSmoothKnockback()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(KnockbackTimerHandle);
    }
}

// ======================================================
// Utility
// ======================================================

FVector USK_GA_AI_HitReaction::CalculateHitDirection(
    const FGameplayEventData* TriggerEventData
) const
{
    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar)
        return FVector::ZeroVector;

    // 기본값: 캐릭터 정면 반대
    FVector Direction = -Avatar->GetActorForwardVector();

    if (TriggerEventData && TriggerEventData->Instigator)
    {
        FVector Dir =
            Avatar->GetActorLocation() -
            TriggerEventData->Instigator->GetActorLocation();

        Dir.Z = 0.f;

        if (!Dir.IsNearlyZero())
        {
            Direction = Dir.GetSafeNormal();
        }
    }

    return Direction;
}