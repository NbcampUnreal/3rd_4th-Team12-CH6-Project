#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SKAN_UnequipWeapon.generated.h"

class UGameplayEffect;

UCLASS()
class SK_API USKAN_UnequipWeapon : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
