// Fill out your copyright notice in the Description page of Project Settings.


#include "SK_GA_SimpleInteract.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_MoveToLocation.h"
#include "Animation/SKBaseAnimInstance.h"
#include "Character/SKPlayerCharacter.h"
#include "Interaction/ActorComponent/SKInteractionComponent.h"
#include "Item/SKInteractableBase.h"

USK_GA_SimpleInteract::USK_GA_SimpleInteract()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void USK_GA_SimpleInteract::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char) { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); return; }
	// if (!Char && Char->HasAuthority()) { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); return; }
	
	UE_LOG(LogTemp, Warning, TEXT("Active Simple Interact Ability, %s"), Char->HasAuthority() ? TEXT("Server") : TEXT("Client"));

	USKInteractionComponent* InteractionComponent = Char->GetInteractionComponent();
	if (!InteractionComponent) { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); return; }
	
	ASKInteractableBase* TargetActor = InteractionComponent->GetCurrentTagetActor();
	CachedTargetActor = TargetActor;
	
	FSKInteractionData& Data = InteractionComponent->GetInteractionData();

	if (Data.InteractionLocation == FVector::Zero())
	{
		UE_LOG(LogTemp, Warning, TEXT("Don't have Interaction Location, %s"), Char->HasAuthority() ? TEXT("Server") : TEXT("Client"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Interaction Location, %s"), *Data.InteractionLocation.ToString())
	}

	// 이동에 걸리는 시간 계산
	FVector TargetLocation = Data.InteractionLocation;
	TargetLocation.Z = Char->GetActorLocation().Z;

	FVector CurrentLocation = Char->GetActorLocation();
	float Distance = FVector::Dist(CurrentLocation, TargetLocation);

	float DesiredSpeed = 300.0f; // 캐릭터 걷기 속도로

	float Duration = Distance / DesiredSpeed;

	// 이동 전 캐릭터 회전
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	FRotator TargetRotation = Direction.Rotation();

	Char->SetActorRotation(TargetRotation);
	
	UAbilityTask_MoveToLocation* MoveTask = UAbilityTask_MoveToLocation::MoveToLocation(this, NAME_None, TargetLocation, Duration, nullptr, nullptr);

	if (MoveTask)
	{
		MoveTask->OnTargetLocationReached.AddDynamic(this, &USK_GA_SimpleInteract::OnMoveCompleted);
		MoveTask->ReadyForActivation();
		USKBaseAnimInstance* AnimClass = Cast<USKBaseAnimInstance>(Char->GetMesh()->GetAnimInstance());
		if (AnimClass)
		{
			AnimClass->SetForceMove(true);
		}
	}
	else
	{
		OnMoveCompleted();
	}
}

void USK_GA_SimpleInteract::OnMoveCompleted()
{
	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());

	USKBaseAnimInstance* AnimClass = Cast<USKBaseAnimInstance>(Char->GetMesh()->GetAnimInstance());
	if (AnimClass)
	{
		AnimClass->SetForceMove(false);
	}
	
	USKInteractionComponent* InteractionComponent = Char->GetInteractionComponent();
	if (!InteractionComponent) { EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true); return; }

	if (CachedTargetActor)
	{
		ISKInteractable::Execute_Interact(CachedTargetActor, Char);
	}
	
	FSKInteractionData& Data = InteractionComponent->GetInteractionData();
		
	// 회전

	Char->SetActorRotation(Data.InteractionRotation);
	
	// 몽타주 재생
	if (Data.InteractMontage)
	{
		
		// Char->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		UAbilityTask_PlayMontageAndWait* PlayAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Interact"), Data.InteractMontage);
		PlayAnimTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
		PlayAnimTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
		PlayAnimTask->ReadyForActivation();
	}
}

void USK_GA_SimpleInteract::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Warning, TEXT("End Simple Interact Ability"));

	ASKPlayerCharacter* Char = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char) return;
	// Char->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	
	USKInteractionComponent* InteractionComponent = Char->GetInteractionComponent();
	if (!InteractionComponent) return;

	InteractionComponent->SetInteractionData(FSKInteractionData());
}

void USK_GA_SimpleInteract::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USK_GA_SimpleInteract::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
