#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Wander.h"
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
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Wander")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Wander")));
}

void USK_GA_AI_Wander::Wander()
{
	WaitMoveComplete();
	
	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(CachedCharacter);
	if (!IsValid(AICharacter))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	FVector StartLocation = AICharacter->GetStartLocation();
	
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!IsValid(NavSystem))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	FNavLocation ResultLocation; 
	bool bSucceed = NavSystem->GetRandomReachablePointInRadius(StartLocation, WanderRadius, ResultLocation);
	if (!bSucceed)
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	FVector TargetLocation = ResultLocation.Location;

	AAIController* AIController = Cast<AAIController>(CachedController);
	if (!IsValid(AIController))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	AIController->MoveToLocation(TargetLocation);
}

void USK_GA_AI_Wander::WaitMoveComplete()
{
	OwnEventTask1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.MoveComplete")),
				nullptr,
				true,
				false
				);
	OwnEventTask1->EventReceived.AddDynamic(this, &USK_GA_AI_Wander::OnWaitMoveCompleteCompleted);
	OwnEventTask1->ReadyForActivation();
}

void USK_GA_AI_Wander::OnWaitMoveCompleteCompleted(FGameplayEventData EventData)
{
	Delay(DelayTime);
}

void USK_GA_AI_Wander::Delay(float DelayDuration)
{
	OwnDelayTask = UAbilityTask_WaitDelay::WaitDelay(
				this,
				DelayDuration
				);
	OwnDelayTask->OnFinish.AddDynamic(this, &USK_GA_AI_Wander::OnDelayCompleted);
	OwnDelayTask->ReadyForActivation();
}

void USK_GA_AI_Wander::OnDelayCompleted()
{
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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Wander Start");
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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Wander End");
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
