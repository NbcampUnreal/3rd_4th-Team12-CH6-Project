#include "SK_GA_InteractionTemplate.h"
#include "Character/SKPlayerCharacter.h"
#include "Animation/SKBaseAnimInstance.h"
#include "Interaction/Interface/SKInteractable.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_MoveToLocation.h"

void USK_GA_InteractionTemplate::SetForceMove(ASKPlayerCharacter* PlayerCharacter, bool ForceMove)
{
	USKBaseAnimInstance* AnimClass = Cast<USKBaseAnimInstance>(PlayerCharacter->GetMesh()->GetAnimInstance());
	if (AnimClass)
	{
		AnimClass->SetForceMove(ForceMove);
	}
}

float USK_GA_InteractionTemplate::GetDistance(ASKPlayerCharacter* PlayerCharacter, FVector TargetLocation)
{
	FVector CurrentLocation = PlayerCharacter->GetActorLocation();
	float Distance = FVector::Dist(CurrentLocation, TargetLocation);

	return Distance;
}

FRotator USK_GA_InteractionTemplate::GetTargetRotation(ASKPlayerCharacter* PlayerCharacter, FVector TargetLocation)
{
	FVector CurrentLocation = PlayerCharacter->GetActorLocation();
	const FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	return Direction.Rotation();
}

void USK_GA_InteractionTemplate::MoveToLocation(ASKPlayerCharacter* PlayerCharacter, FVector TargetLocation, const float Duration)
{
	UAbilityTask_MoveToLocation* MoveTask = UAbilityTask_MoveToLocation::MoveToLocation(this, NAME_None, TargetLocation, Duration, nullptr, nullptr);

	if (MoveTask)
	{
		MoveTask->OnTargetLocationReached.AddDynamic(this, &USK_GA_InteractionTemplate::OnMoveCompleted);
		MoveTask->ReadyForActivation();
		SetForceMove(PlayerCharacter, true);
	}
}

void USK_GA_InteractionTemplate::PlayAnimMontage(UAnimMontage* InteractMontage)
{
	if (InteractMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Interact"), InteractMontage);
		PlayAnimTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
		PlayAnimTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
		PlayAnimTask->ReadyForActivation();
	}
}

void USK_GA_InteractionTemplate::ExecuteTargetInteraction(UObject* TargetActor, AActor* Interactor)
{
	ISKInteractable::Execute_Interact(TargetActor, Interactor);
};