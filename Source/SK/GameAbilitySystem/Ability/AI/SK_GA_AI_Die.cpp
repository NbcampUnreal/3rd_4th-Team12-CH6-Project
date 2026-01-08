#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Die.h"
#include "AbilitySystemComponent.h"
#include "Character/AI/SKAICharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Controller/SKPlayerController.h"
#include "Controller/AI/SKAIController.h"
#include "Utility/DropSubsystem.h"
#include "GameMode/DungeonGameMode.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/SKGameInstance.h"

USK_GA_AI_Die::USK_GA_AI_Die()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Die")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Die")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Die")));
}

void USK_GA_AI_Die::Die(TObjectPtr<UAnimMontage> LocalAnimMontage)
{
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				LocalAnimMontage,
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
	ASKAIController* AIController = Cast<ASKAIController>(CachedController);
	if (!IsValid(AIController))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(AIController->FindClosestTargetTimerHandle);
	
	ASKAICharacterBase* AICharacter = Cast<ASKAICharacterBase>(CachedCharacter);
	if (!IsValid(AICharacter))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	auto* DropSubsystem = GetWorld()->GetSubsystem<UDropSubsystem>();
	if (!IsValid(DropSubsystem))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	DropSubsystem->ProcessDropTable(AICharacter->GetDropTableID(), AICharacter->GetActorLocation());

	FMonsterData MonsterData = AICharacter->GetMonsterData();
	
	int32 GoldToGive = FMath::RandRange(MonsterData.RewardMinGold, MonsterData.RewardMaxGold);
	ADungeonGameMode* GM = GetWorld()->GetAuthGameMode<ADungeonGameMode>();
	if (!IsValid(GM))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	GM->AddGoldToPlayers(GoldToGive);
	
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC)
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	if (SourceASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Boss"))))
	{
		USKGameplayMessageSubsystem* MessageSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USKGameplayMessageSubsystem>();
		if (!IsValid(MessageSubsystem))
		{
			EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
			return;
		}
	
		FSlotVisibilityMessage Message;
		Message.LayoutTag = TAG_UI_Layout_InGame;
		Message.SlotTags.AddTag(TAG_UI_Slot_BossHP);
		Message.bVisible = false;
	
		MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SlotVisible, Message);

		USKGameInstance* SKGI = Cast<USKGameInstance>(AIController->GetGameInstance());
		if (!SKGI)
		{
			EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
			return;
		}
		SKGI->OpenTownEnding();
	}
	
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

	if (UWorld* World = GetWorld())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
		if (ASKPlayerController* SKPC = Cast<ASKPlayerController>(PC))
		{
			if (SKPC->IsLocalController())
			{
				SKPC->ResetLockOn();
			}
		}
	}
	
	AnimMontage = GetAnimMontage("Death");
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
