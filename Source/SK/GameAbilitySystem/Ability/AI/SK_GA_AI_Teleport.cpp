#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Teleport.h"
#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/AI/SKAICharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Controller/AI/SKAIController.h"
#include "GameFramework/Character.h"

USK_GA_AI_Teleport::USK_GA_AI_Teleport()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Teleport")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Teleport")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Teleport")));
}

TObjectPtr<AActor> USK_GA_AI_Teleport::GetTargetActor() const
{
	ASKAIController* AIController = Cast<ASKAIController>(CachedController);
	if (!IsValid(AIController))
	{
		return nullptr;
	}

	TObjectPtr<AActor> TargetActor = AIController->GetTargetActor();

	return TargetActor;
}

FVector USK_GA_AI_Teleport::GetTeleportLocation() const
{
	UWorld* World = GetAvatarActorFromActorInfo()->GetWorld();
	if (!IsValid(World))
	{
		return FVector::ZeroVector;
	}
	
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!IsValid(NavSystem))
	{
		return FVector::ZeroVector;
	}

	AActor* TargetActor = GetTargetActor();
	if (!IsValid(TargetActor))
	{
		return FVector::ZeroVector;
	}
	
	FVector Origin = TargetActor->GetActorLocation();

	ASKAICharacter* AI = Cast<ASKAICharacter>(CachedCharacter);
	if (!IsValid(AI))
	{
		return FVector::ZeroVector;
	}

	float Distance = AI->AttackArea->GetScaledBoxExtent().X;
	
	FNavLocation ResultLocation;

	for (int32 i = 0; i < 10; i++)
	{
		float RandomAngle = FMath::FRandRange(0.f, 360.f);
		float RandomRadian = FMath::DegreesToRadians(RandomAngle);
		
		FVector RelativePos(
			FMath::Cos(RandomRadian) * Distance,
			FMath::Sin(RandomRadian) * Distance,
			0.f
		);

		FVector TeleportPoint = Origin + RelativePos;
		
		if (NavSystem->ProjectPointToNavigation(TeleportPoint, ResultLocation, FVector(150.f, 150.f, 500.f)))
		{
			float AICapsuleHalfHeight = AI->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			return ResultLocation.Location + FVector(0.f, 0.f, AICapsuleHalfHeight);
		}
	}

	return FVector::ZeroVector; 
}

void USK_GA_AI_Teleport::Teleport(FVector Location)
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!IsValid(SourceASC))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	SetFocus();
	
	FVector SpawnLocation = CachedCharacter->GetActorLocation();
	float AICapsuleHalfHeight = CachedCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SpawnLocation.Z -= AICapsuleHalfHeight;
	
	FGameplayCueParameters Params;
	Params.Location = SpawnLocation;

	SourceASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("AI.Hide"));
	SourceASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.AI.Teleport"), Params);
	
	CachedCharacter->SetActorHiddenInGame(true);
	CachedCharacter->SetActorLocation(Location);

	Delay(1.f);
}

void USK_GA_AI_Teleport::Delay(float DelayDuration)
{
	OwnDelayTask = UAbilityTask_WaitDelay::WaitDelay(
				this,
				DelayDuration
				);
	OwnDelayTask->OnFinish.AddDynamic(this, &USK_GA_AI_Teleport::OnDelayCompleted);
	OwnDelayTask->ReadyForActivation();
}

void USK_GA_AI_Teleport::OnDelayCompleted()
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!IsValid(SourceASC))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	FVector SpawnLocation = CachedCharacter->GetActorLocation();
	float AICapsuleHalfHeight = CachedCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SpawnLocation.Z -= AICapsuleHalfHeight;
	
	FGameplayCueParameters Params;
	Params.Location = SpawnLocation;

	SourceASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.AI.Teleport"), Params);
	SourceASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("AI.Hide"));

	CachedCharacter->SetActorHiddenInGame(false);
	
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Teleport::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommonEventTask->EndTask();
	
	FVector TeleportLocation = GetTeleportLocation();

	if (TeleportLocation == FVector::ZeroVector)
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	Teleport(TeleportLocation);
}

void USK_GA_AI_Teleport::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	ClearFocus();

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (IsValid(SourceASC))
	{
		if (SourceASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("AI.Hide")))
		{
			SourceASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("AI.Hide"));
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
