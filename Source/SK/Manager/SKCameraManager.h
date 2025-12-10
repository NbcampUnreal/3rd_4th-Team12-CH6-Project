// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "SKCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class SK_API ASKCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	ASKCameraManager();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetIsLockedOn(bool ArgIsLockedOn);
	bool ValidateLockOn(AActor* Player);
	void AdjustCameraDistance(float WheelValue);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|LockOn")
	UMaterialInterface* LockOnOverlayMaterial;
	void SetLockedTarget(AActor* NewTarget);
	void OnTargetChanged( AActor* NewTarget);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "SK|LockOn")
	float fOutLineActiveTime = 5.f;

	
	UPROPERTY()
	AActor* LockedTarget;

	bool GetIsLockedOn();
	UPROPERTY()
	AActor* OldTarget;
	
	UPROPERTY(ReplicatedUsing = OnRep_OnLockOnChange)
	bool bIsLockedOn = false;

	UFUNCTION()
	void OnRep_OnLockOnChange();

	UFUNCTION(Server, Reliable)
	void Server_SetLockOnState(bool bLock);

	void Camera_SetLockOnState();

	
	UPROPERTY(EditAnywhere,Category = "SK|LockOn")
	float RotateSpeed = 5.f;
	
	// 회전 속도
	UPROPERTY(EditAnywhere,Category = "SK|LockOn")
	float LockOnInterpSpeed = 7.f;

	//카메라높이
	UPROPERTY(EditAnywhere,Category = "SK|LockOn") 
	float LockOnHeight = 60.f;

	//카메라각도
	UPROPERTY(EditAnywhere,Category = "SK|LockOn")
	float LockOnPitch = 10.f;

	//추적최대거리
	UPROPERTY(EditAnywhere,Category = "SK|LockOn")
	float MaxLockDistance = 2000.f;
	//추적최소거리
	UPROPERTY(EditAnywhere,Category = "SK|LockOn")
	float MinLockDistance = 200.f;
	//카메라 줌 거리 
	UPROPERTY(EditAnywhere,Category = "SK|LockOn")
	float MaxCameraZoom = 1000.f;

	UPROPERTY(VisibleAnywhere, Category = "SK|LockOn")
	float CurrentZoomDistance = 600.f;   // 기본값
	//카메라 줌 거리
	UPROPERTY(EditAnywhere,Category = "SK|LockOn")
	float MinCameraZoom = 200.f;
	
	bool IsTargetObstructed(const FVector& CamLoc, const FVector& TargetLoc);
protected:
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

private:
	
};
