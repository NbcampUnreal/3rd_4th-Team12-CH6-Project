#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Die.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/AI/SKAICharacter.h"
#include "GameFramework/Character.h"
#include "Components/StateTreeAIComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Utility/DropSubsystem.h"

USK_GA_AI_Die::USK_GA_AI_Die()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Die")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	//ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Death")));
}

void USK_GA_AI_Die::WaitDieEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("죽음이벤트대기시작"));

	UAbilityTask_WaitGameplayEvent* EventTask =
			UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.Death")),
				nullptr,
				true,
				false
			);

	EventTask->EventReceived.AddDynamic(this, &USK_GA_AI_Die::OnWaitDieEventCompleted);
	EventTask->ReadyForActivation();
}

void USK_GA_AI_Die::OnWaitDieEventCompleted(FGameplayEventData EventData)
{
	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(CachedCharacter);
	if (!IsValid(AICharacter))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	UAnimMontage* AnimMontage = AICharacter->GetMontages()[1]; // 임시로 일단 1번 인덱스 고정
	if (!IsValid(AnimMontage))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("죽음이벤트받음"));
	Die(AnimMontage);
}

void USK_GA_AI_Die::Die(UAnimMontage* AnimMontage)
{
	CachedController->StopMovement();
	
	UAbilitySystemComponent* SourceASC = CachedActorInfo->AbilitySystemComponent.Get();
	if (!SourceASC)
	{
		return;	
	}

	FGameplayTagContainer CancelTags;
	CancelTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability")));
	FGameplayTagContainer IgnoreTags;
	IgnoreTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Pause")));
	
	SourceASC->CancelAbilities(&CancelTags, &IgnoreTags, this);
	
	UStateTreeAIComponent* ST = CachedController->FindComponentByClass<UStateTreeAIComponent>();
	if (!IsValid(ST))
	{
		return;
	}
	
	ST->StopLogic(TEXT("AI Death"));
	
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AnimMontage,
			1.0f,
			NAME_None,
			false,
			1.0f
		);

	MontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_Die::OnDieCompleted);
	//MontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_Die::OnDieInterrupted);
	//MontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_Die::OnDieCancelled);
	//MontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_Die::OnMontageBlendOut);
	MontageTask->ReadyForActivation();
}

void USK_GA_AI_Die::OnDieCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);

	auto* DropSubsystem = GetWorld()->GetSubsystem<UDropSubsystem>();
	if (!IsValid(DropSubsystem))
	{
		return;
	}
	
	DropSubsystem->ProcessDropTable(1, CachedCharacter->GetActorLocation());
	
	CachedCharacter->Destroy();
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
	
	WaitDieEvent();
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
