#include "GameAbilitySystem/Ability/AI/SK_GA_AI_Breath.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/AI/SKAICharacter.h"
#include "Projectile/SKBaseProjectile.h"

USK_GA_AI_Breath::USK_GA_AI_Breath()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Breath")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Breath")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Breath")));
}

void USK_GA_AI_Breath::Breath(TObjectPtr<UAnimMontage> LocalAnimMontage)
{
	SetFocus();
	
	OwnEventTask1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Attack")),
				nullptr,
				true,
				false
				);
	OwnEventTask1->EventReceived.AddDynamic(this, &USK_GA_AI_Breath::OnAnimNotifyCompleted);
	OwnEventTask1->ReadyForActivation();

	OwnEventTask2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.Hit")),
				nullptr,
				false,
				false
				);
	OwnEventTask2->EventReceived.AddDynamic(this, &USK_GA_AI_Breath::OnWaitHitCompleted);
	OwnEventTask2->ReadyForActivation();
	
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				LocalAnimMontage,
				1.0f,
				"Default",
				true,
				1.0f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_Breath::OnBreathCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_Breath::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_Breath::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_Breath::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_Breath::OnAnimNotifyCompleted(FGameplayEventData EventData)
{
	CurrentAttackType = EventData.EventTag;
	AddDamageMultiplierByAttackType();
	DamageMultiplier += 0.4f;
	
	const FGameplayCueParameters CueParameters;
	
	K2_AddGameplayCueWithParams(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.AI.Breath")), CueParameters);

	UWorld* World = GetAvatarActorFromActorInfo()->GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	if (ProjectileTimerHandle.IsValid())
	{
		return;
	}
	
	World->GetTimerManager().SetTimer(
		ProjectileTimerHandle,      
		this,            
		&USK_GA_AI_Breath::SpawnAndLaunchProjectile, 
		0.1f,              
		true
	);		
}

void USK_GA_AI_Breath::SpawnAndLaunchProjectile()
{
	if (!IsValid(ProjectileClass))
	{
		return;
	}

	FVector SpawnLocation = CachedCharacter->GetMesh()->GetSocketLocation("head_socket");
	FRotator SpawnRotation = CachedCharacter->GetMesh()->GetSocketRotation("head_socket");

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = CachedCharacter;
	SpawnParams.Owner = CachedCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Projectile = GetWorld()->SpawnActor<ASKBaseProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (!IsValid(Projectile))
	{
		return;
	}

	Projectile->LaunchProjectile(SpawnRotation.Vector());
}

void USK_GA_AI_Breath::OnWaitHitCompleted(FGameplayEventData EventData)
{
	HitActor = EventData.Target.Get();
	if (!HitActor.IsValid())
	{
		return;
	}
	
	ApplyDamageToTarget(HitActor);
}

void USK_GA_AI_Breath::OnBreathCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_Breath::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommonEventTask->EndTask();
	
	AnimMontage = GetAnimMontage("Breath");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ProjectileClass = GetProjectileClass("Breath");
	
	Breath(AnimMontage);
}

void USK_GA_AI_Breath::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	ClearFocus();

	UWorld* World = GetAvatarActorFromActorInfo()->GetWorld();
	if (IsValid(World))
	{
		World->GetTimerManager().ClearTimer(ProjectileTimerHandle);
		ProjectileTimerHandle.Invalidate();
	}
	
	K2_RemoveGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.AI.Breath")));
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}