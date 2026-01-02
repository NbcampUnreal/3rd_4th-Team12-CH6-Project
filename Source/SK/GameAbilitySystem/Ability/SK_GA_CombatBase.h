#pragma once

#include "CoreMinimal.h"
#include "SKGameplayAbility.h"
#include "SK_GA_CombatBase.generated.h"

UCLASS()
class SK_API USK_GA_CombatBase : public USKGameplayAbility
{
	GENERATED_BODY()

public:
	USK_GA_CombatBase();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
#pragma region Snap

public:
	void SnapToTarget(ASKPlayerCharacter* SKPlayer);

protected:
	FVector GetSnapLocation(ASKPlayerCharacter* SKPlayer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float BaseDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float DualOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float KatanaOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float TraceDist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float SnapDist;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float TraceRadius;
#pragma endregion
};
