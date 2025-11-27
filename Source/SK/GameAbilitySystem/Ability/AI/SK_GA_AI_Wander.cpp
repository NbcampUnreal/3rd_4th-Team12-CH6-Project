#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Wander.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
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
	if (!IsActive()) 
	{
		return;
	}
	
	Delay(DelayTime);
}

void USK_GA_AI_Wander::Delay(float DelayDuration)
{
	UAbilityTask_WaitDelay* DelayTask =
			UAbilityTask_WaitDelay::WaitDelay(
				this,
				DelayDuration
			);
	
	DelayTask->OnFinish.AddDynamic(this, &USK_GA_AI_Wander::OnDelayCompleted);
	DelayTask->ReadyForActivation();
}

void USK_GA_AI_Wander::OnDelayCompleted()
{
	if (!IsActive()) 
	{
		return;
	}
	
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
	
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return;
	}
	
	UAbilitySystemComponent* SourceASC = ActorInfo->AbilitySystemComponent.Get();
	if (!SourceASC)
	{
		return;	
	}
	
	SourceASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Condition.Perception")));
	
	if (!IsActive()) 
	{
		return;
	}
	
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
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
