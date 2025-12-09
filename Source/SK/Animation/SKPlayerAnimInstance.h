#pragma once

#include "CoreMinimal.h"
#include "SKBaseAnimInstance.h"
#include "Controller/SKPlayerController.h"
#include "SKPlayerAnimInstance.generated.h"

class USKActionComponent;
class ASKPlayerCharacter;

UCLASS()
class SK_API USKPlayerAnimInstance : public USKBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	ASKPlayerCharacter* OwnerPlayer;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	USKActionComponent* ActionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EMoveDirection CurrentMoveDirection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsLockedOn;
};
