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
	//데이터에셋이나 데이터테이블에 넣어 몬스터마다 각자 거 쓸 수 있게 수정 필요.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ASKBaseProjectile> ProjectileClass;

	//데이터에셋이나 데이터테이블에 넣어 몬스터마다 각자 거 쓸 수 있게 수정 필요.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FName ProjectileSpawnSocketName = TEXT("hand_r");
	
	
public:
	USK_GA_AI_ProjectileAttack();

	void SpawnProjectile();

	void LaunchProjectile();

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
