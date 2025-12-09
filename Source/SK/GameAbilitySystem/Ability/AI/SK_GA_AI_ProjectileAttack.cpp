#include "GameAbilitySystem/Ability/AI/SK_GA_AI_ProjectileAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/CapsuleComponent.h"
#include "Controller/AI/SKAIController.h"
#include "Projectile/SKBaseProjectile.h"
#include "GameFramework/Character.h"

USK_GA_AI_ProjectileAttack::USK_GA_AI_ProjectileAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.ProjectileAttack")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.ProjectileAttack")));
}

void USK_GA_AI_ProjectileAttack::WaitAnimNotify()
{
	OwnEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(TEXT("Event.AnimNotify")),
				nullptr,
				true,
				false
				);
	OwnEventTask->EventReceived.AddDynamic(this, &USK_GA_AI_ProjectileAttack::OnWaitAnimNotifyCompleted);
	OwnEventTask->ReadyForActivation();
}

void USK_GA_AI_ProjectileAttack::SpawnProjectile()
{
	if (!ProjectileClass)
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	FVector SpawnLocation = CachedCharacter->GetActorLocation();
	SpawnLocation += CachedCharacter->GetActorForwardVector() * (CachedCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() + 200.f);
	const FRotator SpawnRotation = CachedCharacter->GetControlRotation(); 

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = CachedCharacter;
	SpawnParams.Owner = CachedCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Projectile = GetWorld()->SpawnActor<ASKBaseProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
}

void USK_GA_AI_ProjectileAttack::LaunchProjectile(TObjectPtr<UAnimMontage> AnimMontage)
{
	WaitAnimNotify();
	
	SpawnProjectile();
	
	if (!IsValid(Projectile))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	OwnMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				AnimMontage,
				1.f,
				NAME_None,
				true,
				1.f
				);
	OwnMontageTask->OnCompleted.AddDynamic(this, &USK_GA_AI_ProjectileAttack::OnLaunchProjectileCompleted);
	//OwnMontageTask->OnInterrupted.AddDynamic(this, &USK_GA_AI_JumpRush::OnMontageInterrupted);
	//OwnMontageTask->OnCancelled.AddDynamic(this, &USK_GA_AI_JumpRush::OnMontageCancelled);
	//OwnMontageTask->OnBlendOut.AddDynamic(this, &USK_GA_AI_ProjectileAttack::OnMontageBlendOut);
	OwnMontageTask->ReadyForActivation();
}

void USK_GA_AI_ProjectileAttack::OnWaitAnimNotifyCompleted(FGameplayEventData EventData)
{
	ASKAIController* AIController = Cast<ASKAIController>(CachedController);
	if (!IsValid(AIController))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}
	
	const FVector LaunchDirection = AIController->GetTargetDirection();
		
	Projectile->LaunchProjectile(LaunchDirection);
}

void USK_GA_AI_ProjectileAttack::OnLaunchProjectileCompleted()
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, false);
}

void USK_GA_AI_ProjectileAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommonEventTask->EndTask();

	TObjectPtr<UAnimMontage> AnimMontage = GetAnimMontage("ProjectileAttack");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	LaunchProjectile(AnimMontage);
}

void USK_GA_AI_ProjectileAttack::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
