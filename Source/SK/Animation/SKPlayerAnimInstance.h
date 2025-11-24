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

	void PlayLeftAttackAnim();
	
	void SetComboIndex(int32 ComboIndex);
	void SetIsAttacking(bool isAttacking);

	TArray<UAnimMontage*> GetLeftATKMontage();
	UAnimMontage* GetLeftATKMontageIndex();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimMontage")
	TArray<UAnimMontage*> LeftAttackMontages;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimMontage")
	TArray<UAnimMontage*> RightAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimMontage")
	TArray<UAnimMontage*> HitMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimMontage")
	TArray<UAnimMontage*> DashMontages;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bLeftAttacking : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 LeftComboIndex = 0;
};
