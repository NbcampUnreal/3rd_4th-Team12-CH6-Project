#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/AI/SK_GA_AI_BaseCombat.h"
#include "SK_GA_AI_ProjectileAttack.generated.h"

class ASKBaseProjectile;

UCLASS()
class SK_API USK_GA_AI_ProjectileAttack : public USK_GA_AI_BaseCombat
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TObjectPtr<ASKBaseProjectile> Projectile;
	
protected:
	//데이터에셋에 넣어 몬스터마다 각자 거 쓸 수 있게 수정 필요.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ASKBaseProjectile> ProjectileClass;

	
public:
	USK_GA_AI_ProjectileAttack();

	void WaitAnimNotify();
	
	void SpawnProjectile();

	void LaunchProjectile(TObjectPtr<UAnimMontage> AnimMontage);

	UFUNCTION()
	void OnWaitAnimNotifyCompleted(FGameplayEventData EventData);
	
	UFUNCTION()
	void OnLaunchProjectileCompleted();
	
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;
};
