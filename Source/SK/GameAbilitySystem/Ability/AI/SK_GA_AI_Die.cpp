#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Die.h"
#include "Character/AI/SKAICharacter.h"
#include "Components/StateTreeAIComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Utility/DropSubsystem.h"
#include "GameMode/DungeonGameMode.h"

USK_GA_AI_Die::USK_GA_AI_Die()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Die")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	//ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Death")));
}

void USK_GA_AI_Die::Die(TObjectPtr<UAnimMontage> AnimMontage)
{
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				AnimMontage,
				1.0f,
				NAME_None,
				false,
				1.0f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_Die::OnDieCompleted);
	//MontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_Die::OnDieInterrupted);
	//MontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_Die::OnDieCancelled);
	//MontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_Die::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_Die::OnDieCompleted()
{
	auto* DropSubsystem = GetWorld()->GetSubsystem<UDropSubsystem>();
	if (!IsValid(DropSubsystem))
	{
		return;
	}

	ASKAICharacterBase* AICharacter = Cast<ASKAICharacterBase>(CachedCharacter);
	
	DropSubsystem->ProcessDropTable(AICharacter->GetDropTableID(), AICharacter->GetActorLocation());
	
	int32 GoldToGive = FMath::RandRange(AICharacter->GetMonsterData().RewardMinGold, AICharacter->GetMonsterData().RewardMaxGold);
	ADungeonGameMode* GM = GetWorld()->GetAuthGameMode<ADungeonGameMode>();
	if (!GM) return;
	GM->AddGoldToPlayers(GoldToGive);
	
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
	
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

	CommonEventTask->EndTask();

	TObjectPtr<UAnimMontage> AnimMontage = GetAnimMontage("Death");
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
