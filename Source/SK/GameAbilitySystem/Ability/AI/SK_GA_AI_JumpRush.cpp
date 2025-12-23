#include "GameAbilitySystem/Ability/AI/SK_GA_AI_JumpRush.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionJumpForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

USK_GA_AI_JumpRush::USK_GA_AI_JumpRush()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.JumpRush")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.JumpRush")));
}

void USK_GA_AI_JumpRush::JumpRush(TObjectPtr<AActor> TargetActor, TObjectPtr<UAnimMontage> AnimMontage)
{
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	if (!IsValid(TargetCharacter))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	FVector StartLocation = CachedCharacter->GetActorLocation();
	FVector EndLocation = TargetCharacter->GetActorLocation();
	FVector ToTargetVector = EndLocation - StartLocation;
	FRotator JumpRotation = ToTargetVector.GetSafeNormal2D().Rotation();
	
	float CapsuleRadiusSum = CachedCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() + TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float Distance = FVector(ToTargetVector.X, ToTargetVector.Y, 0.f).Length() - CapsuleRadiusSum;
	float Height = FMath::Clamp(Distance * 0.1f, 40.f, 100.f);
	//float Duration = FMath::Clamp(Distance / 800.f, 0.5f, 1.5f);
	float Duration = AnimMontage->GetPlayLength();

	//float MontageRate = AnimMontage->GetPlayLength() / Duration;

	OwnEventTask1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.Hit")),
				nullptr,
				true,
				false
				);
	OwnEventTask1->EventReceived.AddDynamic(this, &USK_GA_AI_JumpRush::OnHitCompleted);
	OwnEventTask1->ReadyForActivation();
	
	OwnJumpTask = UAbilityTask_ApplyRootMotionJumpForce::ApplyRootMotionJumpForce(
				this,
				"JumpRush",
				JumpRotation,
				Distance,
				Height,
				Duration,
				0.1,
				true,
				ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
				FVector::ZeroVector,
				0.f,
				nullptr,
				nullptr
				);
	OwnJumpTask->OnLanded.AddDynamic(this, &USK_GA_AI_JumpRush::OnJumpRushCompleted);
	OwnJumpTask->ReadyForActivation();
	
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				AnimMontage,
				1.f,//MontageRate,
				NAME_None,
				true,
				1.f
				);		
	//OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_JumpRush::OnMeleeCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_JumpRush::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_JumpRush::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_JumpRush::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();

	//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
	// 1. 내비게이션 투영 (아까 질문하신 안전지대 확보)
	// C++에서는 UNavigationSystemV1을 사용합니다.
	/*
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation ProjectedLocation;
	if (NavSystem && NavSystem->ProjectPointToNavigation(EndPos, ProjectedLocation))
	{
		EndPos = ProjectedLocation.Location; // 안전한 위치로 보정
	}
	*/
	

	/*
	FVector AILocation = CachedCharacter->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	FVector TargetVelocity = TargetActor->GetVelocity();

	float PredictionTime = AnimMontage->GetPlayLength() * 0.6f;
	
	FVector PredictedLocation = TargetLocation + TargetVelocity * PredictionTime;
	PredictedLocation.Z = TargetLocation.Z;

	FVector PredictedVector = PredictedLocation - AILocation;

	FVector PredictedHorizontalVector = PredictedVector;
	PredictedHorizontalVector.Z = 0.0f;
	
	float WarpDistance = PredictedHorizontalVector.Length();
	float MaxDistance = FVector::Distance(AILocation, TargetLocation) + 200.0f;

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
	
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				AnimMontage,
				1.0f,
				NAME_None,
				false,
				1.0f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_JumpRush::OnJumpRushCompleted);
	//Task->OnInterrupted.AddDynamic(this, &USK_GA_Melee::OnMontageInterrupted);
	//Task->OnCancelled.AddDynamic(this, &USK_GA_Melee::OnMontageCancelled);
	//Task->OnBlendOut.AddDynamic(this, &USK_GA_Melee::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
	*/
}

void USK_GA_AI_JumpRush::OnHitCompleted(FGameplayEventData EventData)
{
	HitActor = EventData.Target.Get();
	if (!HitActor.IsValid())
	{
		return;
	}
	
	ApplyDamageToTarget(HitActor);
}

void USK_GA_AI_JumpRush::OnJumpRushCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_JumpRush::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommonEventTask->EndTask();

	TObjectPtr<AActor> TargetActor = GetTargetActor();
	if (!IsValid(TargetActor))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	TObjectPtr<UAnimMontage> AnimMontage = GetAnimMontage("JumpRush");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	JumpRush(TargetActor, AnimMontage);
}

void USK_GA_AI_JumpRush::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}