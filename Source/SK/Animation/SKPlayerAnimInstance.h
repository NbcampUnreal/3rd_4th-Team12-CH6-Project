#pragma once

#include "CoreMinimal.h"
#include "SKBaseAnimInstance.h"
#include "Controller/SKPlayerController.h"
#include "SKPlayerAnimInstance.generated.h"

UCLASS()
class SK_API USKPlayerAnimInstance : public USKBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EMoveDirection CurrentMoveDirection;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	bool bIsLockOn;
};
