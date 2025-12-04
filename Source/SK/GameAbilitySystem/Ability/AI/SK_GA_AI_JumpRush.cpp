#include "GameAbilitySystem/Ability/AI/SK_GA_AI_JumpRush.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

USK_GA_AI_JumpRush::USK_GA_AI_JumpRush()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.JumpRush")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.JumpRush")));
}

void USK_GA_AI_JumpRush::JumpRush(TObjectPtr<AActor> TargetActor) const
{
	FVector StartLocation = CachedCharacter->GetActorLocation();
	FVector EndLocation = TargetActor->GetActorLocation();
    
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

	// 2. 발사 벡터 계산 (Custom Arc 사용)
	FVector LaunchVelocity;
	bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this,
		LaunchVelocity,
		StartLocation,
		EndLocation,
		0.0f,  // Gravity (0 = 월드 중력)
		0.5f   // Arc (0.5 = 적당한 포물선)
	);

	// 3. 캐릭터 발사
	if (bSuccess)
	{
		// 발사 전 회전 보정 (타겟 바라보기)
		FVector LookDirection = StartLocation - EndLocation;
		LookDirection.Z = 0.0f; // 수평 회전만
		CachedCharacter->SetActorRotation(LookDirection.Rotation());

		// LaunchCharacter 함수 호출 (ACharacter 클래스 멤버 함수)
		// bXYOverride: true, bZOverride: true
		CachedCharacter->LaunchCharacter(LaunchVelocity, true, true);
	}
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

void USK_GA_AI_JumpRush::OnJumpRushCompleted()
{
	FRotator CorrectRotation = CachedCharacter->GetActorRotation();
	CorrectRotation.Pitch = 0.f;
	CorrectRotation.Roll = 0.f;
	CachedCharacter->SetActorRotation(CorrectRotation);
	// 필요하다면 커스텀 틱 태스크에서 보간보정 필요
	
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

	/*
	TObjectPtr<UAnimMontage> AnimMontage = GetAnimMontage("JumpRush");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}*/

	TObjectPtr<AActor> TargetActor = GetTargetActor();
	if (!IsValid(TargetActor))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "JumpRush Activated");
	JumpRush(TargetActor);
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