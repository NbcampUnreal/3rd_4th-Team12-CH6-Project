#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Base.h"
#include "GameFramework/Character.h"
#include "Components/StateTreeAIComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

USK_GA_AI_Base::USK_GA_AI_Base()
{
	
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
	if (OwnEventTask)
	{
		if (OwnEventTask->IsActive())
		{
			OwnEventTask->EndTask();
		}
	}
	if (OwnMontageTask)
	{
		if (OwnMontageTask->IsActive())
		{
			UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
			if (!IsValid(SourceASC))
			{
				return;
			}

			SourceASC->CurrentMontageStop();
			
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

	CachedController->StopMovement();

	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
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
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	ACharacter* AvatarCharacter = Cast<ACharacter>(AvatarActor);
	if (!IsValid(AvatarCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	CachedCharacter = AvatarCharacter;
	
	AController* Controller = AvatarCharacter->GetController();
	if (!IsValid(Controller))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	CachedController = Controller;

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
	// 나중에 실제 캔슬될 일이 생기면 그땐 조건 지워야 함.
	if (!bWasCancelled)
	{
		UStateTreeAIComponent* ST = CachedController->FindComponentByClass<UStateTreeAIComponent>();
		if (!IsValid(ST))
		{
			return;
		}

		ST->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("Event.EndAbility"));
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
