// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SKPlayerController.generated.h"


UENUM(BlueprintType)
enum class EMoveDirection : uint8
{
	None        UMETA(DisplayName = "None"),
	Forward     UMETA(DisplayName = "Forward"),
	Backward    UMETA(DisplayName = "Backward"),
	Left        UMETA(DisplayName = "Left"),
	Right       UMETA(DisplayName = "Right")
};

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPawnPossessedSignature, APawn* /*PossessedPawn*/);
/**
 * 
 */
UCLASS()
class SK_API ASKPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASKPlayerController();

	FOnPawnPossessedSignature OnPawnPossessed;

	//던전 입장 (Host 전용)
	UFUNCTION(BlueprintCallable)
	void EnterDungeon();

	//마을 복귀 (Host 전용)
	UFUNCTION(BlueprintCallable)
	void ReturnToTown();

	//파티 해제 및 로컬 마을 복귀
	UFUNCTION(BlueprintCallable)
	void LeaveSessionAndReturnToLocalTown();

	UPROPERTY()
	AActor* CurrentTarget = nullptr;
	// 락온 상태
	bool bIsLockedOn = false;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	
#pragma region IMA_AND_IA

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> DashAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
    TObjectPtr<UInputAction> Interaction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> LeftAttackAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> RightAttackAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> MouseWheelAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> QuickSlotAction_00;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> QuickSlotAction_01;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> QuickSlotAction_02;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> QuickSlotItem_00;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> QuickSlotItem_01;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SK|Input")
	TObjectPtr<UInputAction> QuickSlotItem_02;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SK|LockOn")
	float LockOnRadius = 1500.f;
private:
	void Dash(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void StopJumping();
	void StartSprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void LeftAttack(const FInputActionValue& Value);
	void RightAttack(const FInputActionValue& Value);
	void Active_MouseWheel(const FInputActionValue& Value);
	void Active_QuickSlotAction_00(const FInputActionValue& Value);
	void Active_QuickSlotAction_01(const FInputActionValue& Value);
	void Active_QuickSlotAction_02(const FInputActionValue& Value);
	void Active_QuickSlotItem_00(const FInputActionValue& Value);
	void Active_QuickSlotItem_01(const FInputActionValue& Value);
	void Active_QuickSlotItem_02(const FInputActionValue& Value);

	AActor* FindNearestTarget();
	void SetLockOnTarget(AActor* NewTarget);
	void UpdateCameraManagerTarget();
private:

#pragma	endregion
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EMoveDirection CurrentMoveDirection;

	UFUNCTION(BlueprintCallable)
	static EMoveDirection GetClosestMoveDirection(const FVector2D& InputVector);
};