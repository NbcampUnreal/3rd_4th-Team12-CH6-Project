#include "GameAbilitySystem/Ability/AI/SK_GA_AI_ProjectileAttack.h"
#include "Projectile/SKBaseProjectile.h"
#include "GameFramework/Character.h"

USK_GA_AI_ProjectileAttack::USK_GA_AI_ProjectileAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Melee")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Alive")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Stunned")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Melee")));
}

void USK_GA_AI_ProjectileAttack::SpawnProjectile()
{
	if (!ProjectileClass)
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	const FVector SpawnLocation = CachedCharacter->GetMesh()->GetSocketLocation(ProjectileSpawnSocketName);
	const FRotator SpawnRotation = CachedCharacter->GetControlRotation(); 

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = CachedCharacter;
	SpawnParams.Owner = CachedCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Projectile = GetWorld()->SpawnActor<ASKBaseProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
}

void USK_GA_AI_ProjectileAttack::LaunchProjectile()
{
	if (!IsValid(Projectile))
	{
		EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
		return;
	}

	const FVector LaunchDirection = Projectile->GetActorRotation().Vector();
		
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

	//CommonEventTask->EndTask();

	TObjectPtr<UAnimMontage> AnimMontage = GetAnimMontage("Melee");
	if (!IsValid(AnimMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	//Melee(AnimMontage);
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
