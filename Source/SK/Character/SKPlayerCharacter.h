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
	
	bool bIsSprinting =false;

	FTimerHandle MovementCheckTimer;

#pragma region Interaction
	UFUNCTION(BlueprintCallable)
	void TraceForInteraction();
	
#pragma endregion Interaction
};
