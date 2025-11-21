#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Die.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/AI/SKAICharacter.h"
#include "GameFramework/Character.h"
#include "Components/StateTreeAIComponent.h"

USK_GA_AI_Die::USK_GA_AI_Die()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Die")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	//ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Death")));
}

void USK_GA_AI_Die::Die(UAnimMontage* AnimMontage)
{
}

void USK_GA_AI_Die::OnDieCompleted()
{
	UAbilitySystemComponent* SourceASC = CachedActorInfo->AbilitySystemComponent.Get();
	if (!SourceASC)
	{
		return;	
	}

	UStateTreeAIComponent* ST = CachedController->FindComponentByClass<UStateTreeAIComponent>();
	if (!IsValid(ST))
	{
		return;
	}

	ACharacter* Character = CachedCharacter;
	
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);

	SourceASC->CancelAllAbilities();
	
	ST->StopLogic(TEXT("AI Death"));
	
	Character->Destroy();
}

void USK_GA_AI_Die::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(CachedCharacter);
	if (!IsValid(AICharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* AnimMontage = AICharacter->GetMontages()[1]; // 임시로 일단 1번 인덱스 고정
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Die(AnimMontage);
}

void USK_GA_AI_Die::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
