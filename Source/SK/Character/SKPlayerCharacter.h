// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/SKCharacterBase.h"
#include "Interaction/Interface/SKInteractable.h"
#include "SKPlayerCharacter.generated.h"


USTRUCT(BlueprintType)
struct FSKComboState
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGameplayTag WeaponTag;

	UPROPERTY()
	FGameplayTag AttackTypeTag; // 좌,우클릭 스킬

	UPROPERTY()
	int32 ComboIndex = 0;

	UPROPERTY()
	bool bIsAttacking = false; //현재공격중인지

	UPROPERTY()
	bool bCanNextCombo = false; //넘어갈수있는지 공격인지 체크

	UPROPERTY()
	bool bBufferedAttack = false;
};

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
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void SetSprinting(bool bSprinting);

	void UpdateMovementTag();
	void UpdateMovementTag_ATK(FGameplayTag ATKTag, bool Enable = true);

	void SetPlayerStateTag();

#pragma region Combo

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Battle")
	// FGameplayTag CurrentWeaponTag;
	UPROPERTY(ReplicatedUsing = OnRep_ComboState)
	FSKComboState ComboState;


	bool bBufferedAttack = false; // 입력 버퍼링 플래그

	// 지난 프레임의 소켓 위치 저장용
	TArray<FName> CurrentWeaponTraceSockets;
	TArray<FVector> PreviousSocketLocations;

	//====================FUNC================================//
	UFUNCTION(BlueprintCallable, Category = "SK|Battle")
	void OnRep_ComboState();

	int32 GetComboIndex();
	bool GetIsAttacking();
	void ResetComboIndex(int32 ArgComboIndex = 0);
	void ResetComboState();
	void IncreaseComboIndex(bool bLeft = true);
	void OnATKEndNotify(bool bLeft = true);
	bool CheckMaxComboIndex(bool bLeft = true);

	FGameplayTag GetLeftATKTag() const;

	void UpdateAnimInstanceComboState();
	//CurrentTag 바꿔주는 함수
	void SetWeapon(FGameplayTag NewWeaponTag);
	void SetTraceSocket();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<AActor*> GetHitActors();
	void ClearHitActor();

	void ActivateLeftAttackGA();
	void OnLeftATKInput();
	void StartAttackTrace();
	void StopAttackTrace();
	void PerformWeaponTrace(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void Server_LeftAttackInput();
	UFUNCTION(Server, Reliable)
	void Server_OnATKEndNotify();
	UFUNCTION(Server, Reliable)
	void Server_Notify_StopAttackTrace();

#pragma endregion

protected:
	virtual void OnRep_PlayerState() override;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

private:
	void SetLooseTag(UAbilitySystemComponent* ASC, const FGameplayTag& Tag, bool bEnable);


#pragma region PlayerAnimState
	bool bIsSprinting = false;
	FTimerHandle MovementCheckTimer;

#pragma endregion

#pragma region Weapon_Collision


	bool bIsTracing = false;

	UPROPERTY()
	TArray<AActor*> HitActors;


#pragma endregion

#pragma region Interaction

public:
	UFUNCTION(Server, Reliable)
	void Server_TryInteract(AActor* Target);

	UFUNCTION(Client, Reliable)
	void Client_PlayPickupSound(USoundBase* PickupSound);

	UPROPERTY(BlueprintReadWrite)
	FSKInteractionData CurrentInteractionData;
#pragma endregion
};


