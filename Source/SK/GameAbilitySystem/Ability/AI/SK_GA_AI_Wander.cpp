#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Wander.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Character/AI/SKAICharacter.h"

USK_GA_AI_Wander::USK_GA_AI_Wander()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Wander")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	//ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Action.Melee")));
}

void USK_GA_AI_Wander::Wander()
{
	WaitMoveCompleteEvent();
	
	WaitPerceptionEvent();
	
	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(CachedCharacter);
	if (!IsValid(AICharacter))
	{
		return;
	}

	FVector StartLocation = AICharacter->GetActorLocation();
	
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!IsValid(NavSystem))
	{
		return;
	}
	
	FNavLocation ResultLocation; 
	bool bSucceed = NavSystem->GetRandomReachablePointInRadius(StartLocation, WanderRadius, ResultLocation);
	if (!bSucceed)
	{
		return;
	}

	FVector TargetLocation = ResultLocation.Location;

	AAIController* AIController = Cast<AAIController>(CachedController);
	if (!IsValid(AIController))
	{
		return;
	}
	
	AIController->MoveToLocation(TargetLocation);
}

void USK_GA_AI_Wander::WaitMoveCompleteEvent()
{
	UAbilityTask_WaitGameplayEvent* EventTask =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.MoveComplete")),
				nullptr,
				true,
				false
			);

	EventTask->EventReceived.AddDynamic(this, &USK_GA_AI_Wander::OnWaitMoveCompleteEventCompleted);
	EventTask->ReadyForActivation();
}

void USK_GA_AI_Wander::OnWaitMoveCompleteEventCompleted(FGameplayEventData EventData)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("엔드어빌리티"));
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Wander::WaitPerceptionEvent()
{
	UAbilityTask_WaitGameplayEvent* EventTask =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.Perception")),
				nullptr,
				true,
				false
			);

	EventTask->EventReceived.AddDynamic(this, &USK_GA_AI_Wander::OnWaitPerceptionEventCompleted);
	EventTask->ReadyForActivation();
}

void USK_GA_AI_Wander::OnWaitPerceptionEventCompleted(FGameplayEventData EventData)
{
	CachedController->StopMovement();
}

void USK_GA_AI_Wander::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Wander();
}

void USK_GA_AI_Wander::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
