// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "SkAnimInstance_Axe.generated.h"


class ASKPlayerCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class SK_API USkAnimInstance_Axe : public UAnimInstance
{
	GENERATED_BODY()
public:

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// void PlayHitAnim(FGameplayTag Direction);
	// void HitAnimEnd();

	void PlayLeftAttackAnim();
	void EndLeftAttackAnim();
	
	void SetComboIndex(int32 ComboIndex);
	void SetIsAttacking(bool isAttacking);
	//void SetCanNextCombo(bool bCanNextCombo);

	UAnimMontage* GetLeftATKMontage();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimMontage")
	TArray<UAnimMontage*> LeftAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimMontage")
	TArray<UAnimMontage*> RightAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimMontage")
	TArray<UAnimMontage*> HitMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AnimMontage")
	TArray<UAnimMontage*> DashMontages;

protected:
	UPROPERTY()
	TObjectPtr<ASKPlayerCharacter> OwnerCharacter; //TFDCharacterBase

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bShouldMove : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsFalling : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bLeftAttacking : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 LeftComboIndex = 0;
	
	
};
