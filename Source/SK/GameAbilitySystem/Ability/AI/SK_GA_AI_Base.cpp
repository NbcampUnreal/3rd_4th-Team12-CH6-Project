#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Base.h"
#include "GameFramework/Character.h"
#include "Components/StateTreeAIComponent.h"

USK_GA_AI_Base::USK_GA_AI_Base()
{
	
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
}

void USK_GA_AI_Base::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
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
