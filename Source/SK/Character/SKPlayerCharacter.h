// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SKCharacterBase.h"
#include "SKPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SK_API ASKPlayerCharacter : public ASKCharacterBase
{
	GENERATED_BODY()

public:
	ASKPlayerCharacter();

	virtual void BeginPlay() override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void SetSprinting(bool bSprinting);

	void UpdateMovementTag();

#pragma region AnimState

	UFUNCTION(BlueprintCallable, Category = "SK|Battle")
	void ResetLeftComboState(FGameplayTag WeaponTag = FGameplayTag());

	UFUNCTION(BlueprintCallable, Category = "SK|Battle")
	bool GetIsLeftAttackingByTag() const;
	UFUNCTION(BlueprintCallable, Category = "SK|Battle")
	int GetIsLeftComboIndexByTag() const;
	UFUNCTION(BlueprintCallable, Category = "SK|Battle")
	void IncreseLeftComboIndex();
	// void LeftAttack();

	FGameplayTag GetLeftATKTag() const;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Battle")
	FGameplayTag CurrentWeaponTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Battle")
	TMap<FGameplayTag, int> ComboIndexMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Battle")
	TMap<FGameplayTag, bool> IsAttackingMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Battle")
	TMap<FGameplayTag, bool> CanNextComboMap;


#pragma endregion

protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

private:
	void SetPlayerStateTag();

#pragma region PlayerAnimState
	bool bIsSprinting = false;
	FTimerHandle MovementCheckTimer;


#pragma endregion
};
