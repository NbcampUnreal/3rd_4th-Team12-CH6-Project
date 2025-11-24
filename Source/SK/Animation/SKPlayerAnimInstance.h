#pragma once

#include "CoreMinimal.h"
#include "SKBaseAnimInstance.h"
#include "SKPlayerAnimInstance.generated.h"

UCLASS()
class SK_API USKPlayerAnimInstance : public USKBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
};
