#pragma once

#include "CoreMinimal.h"
#include "GameAbilitySystem/Ability/SKGameplayAbility.h"
#include "SK_GA_Dodge.generated.h"

class ASKPlayerCharacter;
class USKActionComponent;
class USKCombatComponent;

UCLASS()
class SK_API USK_GA_Dodge : public USKGameplayAbility
{
	GENERATED_BODY()
public:
	USK_GA_Dodge();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility) override;

protected:
	
	UFUNCTION()
	void OnCompleted();

	UFUNCTION()
	void OnCanceled();

	UFUNCTION()
	virtual void PreActivateDodge(USKActionComponent* ActionComponent);

	UFUNCTION()
	FName SetDodgeDirection(ASKPlayerCharacter* PlayerCharacter, USKActionComponent* ActionComponent);
	
	UPROPERTY()
	UAnimMontage* DodgeMontage;

#pragma region Invincible
	UFUNCTION()
	void AddInvincibleTag();

	UFUNCTION()
	void RemoveInvincibleTag();
	
	UPROPERTY()
	FTimerHandle InvincibleStartTimer;
	
	UPROPERTY()
	FTimerHandle InvincibleEndTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	float DodgeStartTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	float DodgeEndTime;
#pragma endregion
};
