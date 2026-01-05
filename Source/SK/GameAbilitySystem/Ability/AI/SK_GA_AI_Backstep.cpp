#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Backstep.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"

USK_GA_AI_Backstep::USK_GA_AI_Backstep()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Backstep")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Backstep")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Backstep")));
}

void USK_GA_AI_Backstep::Backstep()
{
	SetFocus();
	
	WaitMoveComplete();

	AAIController* AIController = Cast<AAIController>(CachedController);
	if (!IsValid(AIController))
	{
		if (OwnEventTask1->IsActive())
		{
			OwnEventTask1->EndTask();
		}
		
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	float BackstepDistance = 1000.0f;

	FVector AILocation = CachedCharacter->GetActorLocation();
	FVector PlayerLocation;
	FVector TargetLocation;
	
	TObjectPtr<AActor> Player = GetTargetActor();
	if (IsValid(Player))
	{
		PlayerLocation = Player->GetActorLocation();

		FVector LookAtVector = PlayerLocation - AILocation;
		LookAtVector.Z = 0.0f; 

		FVector LookAtDirection = LookAtVector.GetSafeNormal(); 

		TargetLocation = AILocation - (LookAtDirection * BackstepDistance); 
	}
	else
	{
		FVector AIForwardVector = CachedCharacter->GetActorForwardVector();
		TargetLocation = AILocation - AIForwardVector * BackstepDistance;
	}

	FNavLocation NewLocation;
	
	if (UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		FVector Extent(100.f, 100.f, 400.f);

		if (NavSystem->ProjectPointToNavigation(TargetLocation, NewLocation, Extent))
		{
			AIController->MoveToLocation(NewLocation.Location);
		}
		else
		{
			if (OwnEventTask1->IsActive())
			{
				OwnEventTask1->EndTask();
			}
			
			EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		}
	}
	else
	{
		if (OwnEventTask1->IsActive())
		{
			OwnEventTask1->EndTask();
		}
		
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
	}
}

void USK_GA_AI_Backstep::WaitMoveComplete()
{
	OwnEventTask1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.MoveComplete")),
				nullptr,
				true,
				false
				);
	OwnEventTask1->EventReceived.AddDynamic(this, &USK_GA_AI_Backstep::OnWaitMoveCompleteCompleted);
	OwnEventTask1->ReadyForActivation();
}

void USK_GA_AI_Backstep::OnWaitMoveCompleteCompleted(FGameplayEventData EventData)
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Backstep::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommonEventTask->EndTask();

	Backstep();
}

void USK_GA_AI_Backstep::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	ClearFocus();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
