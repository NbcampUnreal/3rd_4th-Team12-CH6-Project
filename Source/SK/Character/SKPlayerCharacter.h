// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/SKCharacterBase.h"
#include "MotionWarpingComponent.h"
#include "SKPlayerCharacter.generated.h"

class USKActionComponent;
class USKInteractionComponent;
struct FSKRepComboState;
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
	

	UBattleComponent* GetBattleComponent() const;

	UMotionWarpingComponent* GetMotionWarpingComponent();
	UFUNCTION()
	void OnAnimInitialized();
	void SetTraceSocket();

	void SetLooseTag(const FGameplayTag& Tag, bool bEnable);

	void AdjustSpringArmDistance(float WheelValue);

	UFUNCTION()
	void SetLockOnRotateMode(bool bLockOn);

	UFUNCTION()
	void OnRep_LockOn();
	void SetLockOnState(bool bNewState);
	void ApplyLockOnState();

	UFUNCTION()
	void PlayGuardCounterFlash();
	UFUNCTION()
	void StopGuardCounterFlash();

protected:
	virtual void OnRep_PlayerState() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarping")
	UMotionWarpingComponent* MotionWarpingComp;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	//히트 효과용
	UPROPERTY(EditDefaultsOnly, Category="SK|PostProcess")
	UMaterialInstance* HitPostProcessMI;

	UPROPERTY()
	UMaterialInstanceDynamic* HitPPMID;

	//가드카운터 효과용
	UPROPERTY(EditDefaultsOnly, Category="SK|PostProcess")
	UMaterialInstance* GuardCounterPostProcessMI;
	
	UPROPERTY()
	UMaterialInstanceDynamic* GuardCounterPPMID;

	UPROPERTY()
	int32 GuardCounterBlendIndex = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Battle")
	TObjectPtr<UBattleComponent> BattleComponent;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	UFUNCTION()
	void OnHitConditionTagChanged(const FGameplayTag Tag, int32 NewCount);

	EMovementMode CachedMovementMode;

private:
	FTimerHandle InitASCTimerHandle;
	void TryInitASC();

	void OnDamageTaken(float Damage);
	FTimerHandle HitEffectTimer;
	FTimerHandle GuardCounterFlashTimer;
	void ResetHitEffectTimer();
	float TargetHitAlpha = 0.f;
	float CurrentHitAlpha = 0.f;
	float HitAlphaRiseSpeed = 30.f;   // 올라갈 때
	float HitAlphaFallSpeed = 8.f;    // 내려갈 때

	UPROPERTY(ReplicatedUsing = OnRep_LockOn)
	bool bIsLockedOn;

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
