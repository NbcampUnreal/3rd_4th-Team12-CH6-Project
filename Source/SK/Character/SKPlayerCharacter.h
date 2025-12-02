// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/SKCharacterBase.h"
#include "Interaction/Interface/SKInteractable.h"
#include "SKPlayerCharacter.generated.h"

class USKInteractionComponent;
struct FSKRepComboState;
class USKCombatComponent; 

UCLASS()
class SK_API ASKPlayerCharacter : public ASKCharacterBase
{
	GENERATED_BODY()

public:
	ASKPlayerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PostInitializeComponents() override;
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void SetSprinting(bool bSprinting);

	void UpdateMovementTag();
	void UpdateMovementTag_ATK(FGameplayTag ATKTag, bool Enable = true);

	void SetPlayerStateTag();

	UFUNCTION(BlueprintPure)
	USKCombatComponent* GetCombatComponent() const;
	UFUNCTION()
	void OnAnimInitialized();
	void SetTraceSocket();


protected:
	virtual void OnRep_PlayerState() override;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USKCombatComponent> CombatComponent;
	
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

private:
	FTimerHandle InitASCTimerHandle;
	void TryInitASC();
	void SetLooseTag(UAbilitySystemComponent* ASC, const FGameplayTag& Tag, bool bEnable);


#pragma region PlayerAnimState
	bool bIsSprinting = false;
	FTimerHandle MovementCheckTimer;

#pragma endregion



#pragma region Interaction

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Interaction)
	TObjectPtr<USKInteractionComponent> InteractionComponent;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE USKInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }
	
#pragma endregion

	//Camera
	void LockOnTarget(float DeltaTime);
};
