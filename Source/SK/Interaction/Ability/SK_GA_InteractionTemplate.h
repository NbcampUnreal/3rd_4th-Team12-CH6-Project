#pragma once

#include "CoreMinimal.h"
#include "SK_GA_InteractionBase.h"
#include "SK_GA_InteractionTemplate.generated.h"

class USKBaseAnimInstance;
class ASKPlayerCharacter;

UCLASS(Abstract)
class SK_API USK_GA_InteractionTemplate : public USK_GA_InteractionBase
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	virtual void OnMoveCompleted() PURE_VIRTUAL(USK_GA_InteractionTemplate::OnMoveCompleted);

	UFUNCTION()
	virtual float GetDistance(ASKPlayerCharacter* PlayerCharacter, FVector TargetLocation);
	UFUNCTION()
	virtual FRotator GetTargetRotation(ASKPlayerCharacter* PlayerCharacter, FVector TargetLocation);
	
	UFUNCTION()
	virtual void SetForceMove(ASKPlayerCharacter* PlayerCharacter, bool ForceMove);
	UFUNCTION()
	virtual void MoveToLocation(ASKPlayerCharacter* PlayerCharacter, FVector TargetLocation, const float Duration);
	UFUNCTION()
	virtual void PlayAnimMontage(UAnimMontage* InteractMontage);
	UFUNCTION()
	virtual void ExecuteTargetInteraction(UObject* TargetActor, AActor* Interactor);

};
