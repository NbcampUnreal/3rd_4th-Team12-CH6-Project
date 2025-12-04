#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Rush.h"
#include "MotionWarpingComponent.h"
#include "SK_GA_AI_Melee.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/AI/SKAICharacter.h"
#include "Components/BoxComponent.h"

USK_GA_AI_Rush::USK_GA_AI_Rush()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Rush")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	//ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Death")));
}

void USK_GA_AI_Rush::Rush(UAnimMontage* AnimMontage)
{
	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(CachedCharacter);
	if (!IsValid(AICharacter))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
		return;
	}

	if (!AICharacter->MotionWarpingComponent)
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
		return;
	}

	AActor* TargetActor = GetTargetActor();
	if (!IsValid(TargetActor))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
		return;
	}

	FVector AILocation = AICharacter->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	FVector TargetVelocity = TargetActor->GetVelocity();

	float PredictionTime = AnimMontage->GetPlayLength() * 0.6f;
	
	FVector PredictedLocation = TargetLocation + TargetVelocity * PredictionTime;
	PredictedLocation.Z = TargetLocation.Z;

	FVector PredictedVector = PredictedLocation - AILocation;

	FVector PredictedHorizontalVector = PredictedVector;
	PredictedHorizontalVector.Z = 0.0f;
	
	float WarpDistance = PredictedHorizontalVector.Length();
	float MaxDistance = AICharacter->BoxComponent->GetScaledBoxExtent().X;

	if (WarpDistance > MaxDistance)
	{
		PredictedHorizontalVector = PredictedHorizontalVector.GetSafeNormal() * MaxDistance;
   
		PredictedLocation = AILocation + FVector(
						PredictedHorizontalVector.X, 
						PredictedHorizontalVector.Y, 
						PredictedVector.Z 
		);
	}
	
	FVector PredictedDirection = (PredictedLocation - AILocation);
	PredictedDirection.Z = 0;
	FRotator PredictedRotation = PredictedDirection.Rotation();
	
	FTransform TargetTransform = FTransform::Identity;
	TargetTransform.SetLocation(PredictedLocation);
	TargetTransform.SetRotation(PredictedRotation.Quaternion());
	
	AICharacter->MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform(
		FName("Player"),
		TargetTransform
	);
	
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				AnimMontage,
				1.0f,
				NAME_None,
				false,
				1.0f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_Rush::OnRushCompleted);
	//Task->OnInterrupted.AddDynamic(this, &USK_GA_Melee::OnMontageInterrupted);
	//Task->OnCancelled.AddDynamic(this, &USK_GA_Melee::OnMontageCancelled);
	//Task->OnBlendOut.AddDynamic(this, &USK_GA_Melee::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_Rush::OnRushCompleted()
{
	FRotator CorrectRotation = CachedCharacter->GetActorRotation();
	CorrectRotation.Pitch = 0.f;
	CorrectRotation.Roll = 0.f;
	CachedCharacter->SetActorRotation(CorrectRotation);
	// 필요하다면 커스텀 틱 태스크에서 보간보정 필요
	
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Rush::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommonEventTask->EndTask();

	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(CachedCharacter);
	if (!IsValid(AICharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAnimMontage* AnimMontage = AICharacter->GetMontages()[0]; // 임시로 일단 0번 인덱스 고정
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	Rush(AnimMontage);
}

void USK_GA_AI_Rush::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
