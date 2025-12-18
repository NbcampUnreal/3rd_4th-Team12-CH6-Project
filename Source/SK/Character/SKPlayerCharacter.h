// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/SKCharacterBase.h"
#include "SKPlayerCharacter.generated.h"

class USKActionComponent;
class USKInteractionComponent;
struct FSKRepComboState;
class USKCombatComponent;
class UBattleComponent;

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

	UBattleComponent* GetBattleComponent() const;
	
	UFUNCTION()
	void OnAnimInitialized();
	void SetTraceSocket();

	void SetLockOnState(bool bLock);
	void SetLooseTag(const FGameplayTag& Tag, bool bEnable);

	void AdjustSpringArmDistance(float WheelValue);

	UFUNCTION()
	void SetLockOnRotateMode(bool bLockOn);
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
	
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Battle")
    	TObjectPtr<UBattleComponent> BattleComponent;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

private:
	FTimerHandle InitASCTimerHandle;
	void TryInitASC();


#pragma region PlayerAnimState
	bool bIsSprinting = false;
	FTimerHandle MovementCheckTimer;

#pragma endregion



#pragma region Interaction

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component|Interaction")
	TObjectPtr<USKInteractionComponent> InteractionComponent;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE USKInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }
	
#pragma endregion

#pragma region Action
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component|Action")
	TObjectPtr<USKActionComponent> ActionComponent;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE USKActionComponent* GetActionComponent() const { return ActionComponent; }
	
#pragma endregion

	
public:
	
};
