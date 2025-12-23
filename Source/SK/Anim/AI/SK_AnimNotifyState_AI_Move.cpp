#include "Anim/AI/SK_AnimNotifyState_AI_Move.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Controller/AI/SKAIController.h"

USK_AnimNotifyState_AI_Move::USK_AnimNotifyState_AI_Move()
{
	
}

void USK_AnimNotifyState_AI_Move::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference
	)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	CurrentMontage = MeshComp->GetAnimInstance()->GetCurrentActiveMontage();
	if (!IsValid(CurrentMontage))
	{
		return;
	}

	StartNotify = MeshComp->GetAnimInstance()->Montage_GetPosition(CurrentMontage);
	NotifyDuration = TotalDuration;
	
	AI = Cast<ACharacter>(MeshComp->GetOwner());
	if (!IsValid(AI))
	{
		return;
	}
	
	StartLocation = AI->GetActorLocation();
	
	ASKAIController* AIController = Cast<ASKAIController>(AI->GetController());
	if (!IsValid(AIController))
	{
		return;
	}
	
	TargetLocation = AIController->GetTargetActor()->GetActorLocation();
	TargetLocation.Z = StartLocation.Z;
}

void USK_AnimNotifyState_AI_Move::NotifyTick(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference
	)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!IsValid(CurrentMontage))
	{
		return;
	}
	
	float CurrentMontagePlayTime = MeshComp->GetAnimInstance()->Montage_GetPosition(CurrentMontage);
	float RelativeTime = CurrentMontagePlayTime - StartNotify;
	float Alpha = FMath::Clamp(RelativeTime / NotifyDuration, 0.0f, 1.0f);

	FVector CurrentGoalLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);

	if (!IsValid(AI))
	{
		return;
	}
	
	FVector DeltaMove = CurrentGoalLocation - AI->GetActorLocation();

	Hit.Init();
	AI->GetCharacterMovement()->SafeMoveUpdatedComponent(DeltaMove, AI->GetActorQuat(), true, Hit);
}

void USK_AnimNotifyState_AI_Move::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
	)
{
	if (IsValid(AI))
	{
		if (Hit.bBlockingHit)
		{
			AI->SetActorLocation(Hit.Location);
		}
		else
		{
			AI->SetActorLocation(TargetLocation);
		}
	}
	
	CurrentMontage = nullptr;

	AI = nullptr;
	
	StartNotify = 0.f;

	NotifyDuration = 0.f;

	StartLocation = FVector::ZeroVector;
	
	TargetLocation = FVector::ZeroVector;

	Hit.Init();
		
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
