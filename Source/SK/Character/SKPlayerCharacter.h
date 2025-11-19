// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/SKCharacterBase.h"
#include "Interaction/Interface/SKInteractable.h"
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

	virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void SetSprinting(bool bSprinting);

	void UpdateMovementTag();
	
	UPROPERTY(BlueprintReadWrite)
	FSKInteractionData CurrentInteractionData;
	
#pragma region Weapon_Collision
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<AActor*> GetHitActors();

	void OnLeftATKInput();
	void ActivateLeftAttackGA();
	void StartAttackTrace();
	void StopAttackTrace();
	void ClearHitActor();
	void SetWeapon(FGameplayTag NewWeaponTag);

	// 실제 트레이스
	void PerformWeaponTrace(float DeltaTime);
	
#pragma endregion
	
#pragma region AnimState

	void OnLeftAttackEndNotify();
	bool CheckMaxLeftComboIndex();
	FGameplayTag GetLeftATKTag() const;

	UFUNCTION(BlueprintCallable, Category = "SK|Battle")
	void ResetLeftComboState(FGameplayTag WeaponTag = FGameplayTag());
	UFUNCTION(BlueprintCallable, Category = "SK|Battle")
	bool GetIsLeftAttackingByTag() const;
	UFUNCTION(BlueprintCallable, Category = "SK|Battle")
	int GetIsLeftComboIndexByTag() const;
	UFUNCTION(BlueprintCallable, Category = "SK|Battle")
	void IncreseLeftComboIndex();



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SK|Battle")
	FGameplayTag CurrentWeaponTag;
	//현재콤보인덱스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Battle")
	TMap<FGameplayTag, int> LeftComboIndexMap;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Battle")
	TMap<FGameplayTag, int> RightComboIndexMap;
	//공격중이면 true
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Battle")
	TMap<FGameplayTag, bool> IsAttackingMap;
	//다음공격이 가능한지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Battle")
	TMap<FGameplayTag, bool> CanNextComboMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Battle")
	TMap<FGameplayTag, int32> LeftMaxComboMap;   // 무기별 최대 콤보 수

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Battle")
	TMap<FGameplayTag, int32> RightMaxComboMap; 

	bool bBufferedAttack = false;    // 입력 버퍼링 플래그

	//Tag별 소켓 정보 DT
	UPROPERTY(EditAnywhere, Category="SK|Weapon")
	UDataTable* WeaponDataTable;
	UPROPERTY(VisibleAnywhere, Category="SK|Weapon")
	TArray<FName> CurrentWeaponTraceSockets;
	// 지난 프레임의 소켓 위치 저장용
	TArray<FVector> PreviousSocketLocations;
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

#pragma region Weapon_Collision

	
	bool bIsTracing = false;
	
	UPROPERTY()
	TArray<AActor*> HitActors;
	
#pragma endregion
};
