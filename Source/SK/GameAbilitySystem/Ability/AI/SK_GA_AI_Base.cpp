#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Base.h"
#include "GameFramework/Character.h"
#include "Components/StateTreeAIComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionJumpForce.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToActorForce.h"
#include "Abilities/tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/AI/SKAICharacterBase.h"
#include "Controller/AI/SKAIController.h"

USK_GA_AI_Base::USK_GA_AI_Base()
{

}

TObjectPtr<UAnimMontage> USK_GA_AI_Base::GetAnimMontage(FName MontageName)
{
	TObjectPtr<UAnimMontage>* MapAnimMontage = Montages.Find(MontageName);
	if (!MapAnimMontage)
	{
		return nullptr;
	}
	
	AnimMontage = *MapAnimMontage;
	if (!IsValid(AnimMontage))
	{
		return nullptr;
	}

	return AnimMontage;
}

void USK_GA_AI_Base::WaitEndAbility()
{
	CommonEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.EndAbility")),
				nullptr,
				true,
				false
				);

	CommonEventTask->EventReceived.AddDynamic(this, &USK_GA_AI_Base::OnWaitEndAbilityCompleted);
	CommonEventTask->ReadyForActivation();
}

void USK_GA_AI_Base::OnWaitEndAbilityCompleted(FGameplayEventData EventData)
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
}

void USK_GA_AI_Base::SetFocus() const
{
	ASKAIController*  AIController = Cast<ASKAIController>(CachedController);
	if (IsValid(AIController))
	{
		TObjectPtr<AActor> TargetActor = AIController->GetTargetActor();
		if (IsValid(TargetActor))
		{
			AIController->SetFocus(TargetActor);
		}
	}
}

void USK_GA_AI_Base::ClearFocus() const
{
	ASKAIController* AIController = Cast<ASKAIController>(CachedController);
	if (IsValid(AIController))
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

void USK_GA_AI_Base::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CachedHandle = Handle;
	CachedActorInfo = ActorInfo;
	CachedActivationInfo = ActivationInfo;
	
	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	ACharacter* AvatarCharacter = Cast<ACharacter>(AvatarActor);
	if (!IsValid(AvatarCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedCharacter = AvatarCharacter;
	
	AController* Controller = AvatarCharacter->GetController();
	if (!IsValid(Controller))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	CachedController = Controller;

	ASKAICharacterBase* BaseAI = Cast<ASKAICharacterBase>(CachedCharacter);
	if (!IsValid(BaseAI))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Montages = BaseAI->GetMontages();
	
	WaitEndAbility();
}

void USK_GA_AI_Base::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	UStateTreeAIComponent* ST = CachedController->FindComponentByClass<UStateTreeAIComponent>();
	if (!IsValid(ST))
	{
		return;
	}
	
	CachedController->StopMovement();

	if (OwnEventTask1)
	{
		if (OwnEventTask1->IsActive())
		{
				OwnEventTask1->EndTask();
		}
	}

	if (OwnEventTask2)
	{
		if (OwnEventTask2->IsActive())
		{
			OwnEventTask2->EndTask();
		}
	}

	if (CommonEventTask)
	{
		if (CommonEventTask->IsActive())
		{
			CommonEventTask->EndTask();
		}
	}
		
	if (OwnMontageTask)
	{
		if (OwnMontageTask->IsActive())
		{
			UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
			if (IsValid(SourceASC))
			{
				SourceASC->CurrentMontageStop();
			}

			OwnMontageTask->EndTask();
		}
	}
			
	if (OwnDelayTask)
	{
		if (OwnDelayTask->IsActive())
		{
			OwnDelayTask->EndTask();
		}
	}

	if (OwnRushTask)
	{
		if (OwnRushTask->IsActive())
		{
			OwnRushTask->EndTask();
		}
	}
		
	if (OwnJumpRushTask)
	{
		if (OwnJumpRushTask->IsActive())
		{
			OwnJumpRushTask->EndTask();
		}
	}

	if (OwnFlyRushTask)
	{
		if (OwnFlyRushTask->IsActive())
		{
			OwnFlyRushTask->EndTask();
		}
	}
	
	if (bWasCancelled)
	{
		ST->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("Event.CancelAbility"));
	}
	else
	{
		ST->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("Event.EndAbility"));
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
