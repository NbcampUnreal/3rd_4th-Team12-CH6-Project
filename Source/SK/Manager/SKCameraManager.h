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



	bool ValidateLockOn(AActor* Player);
	void AdjustCameraDistance(float WheelValue);

	UMaterialInterface* Get_OutLineMat();
	float Get_OutLineTime();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|LockOn")
	UMaterialInterface* LockOnOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SK|LockOn")
	float fOutLineActiveTime = 5.f;

	UPROPERTY(EditAnywhere, Category = "SK|LockOn")
	float RotateSpeed = 5.f;

	// 회전 속도
	UPROPERTY(EditAnywhere, Category = "SK|LockOn")
	float LockOnInterpSpeed = 7.f;

	//카메라높이
	UPROPERTY(EditAnywhere, Category = "SK|LockOn")
	float LockOnHeight = 60.f;

	//카메라각도
	UPROPERTY(EditAnywhere, Category = "SK|LockOn")
	float LockOnPitch = 10.f;

	UPROPERTY(EditAnywhere, Category="SK|LockOn")
	float LockOnLookOffsetZ = 150.f;

	UPROPERTY(EditAnywhere, Category="SK|LockOn")
	float LockOnAngleDeg = 15.f;

	//추적최대거리
	UPROPERTY(EditAnywhere, Category = "SK|LockOn")
	float MaxLockDistance = 2000.f;
	//추적최소거리
	UPROPERTY(EditAnywhere, Category = "SK|LockOn")
	float MinLockDistance = 200.f;
	//카메라 줌 거리 
	UPROPERTY(EditAnywhere, Category = "SK|LockOn")
	float MaxCameraZoom = 1000.f;

	UPROPERTY(VisibleAnywhere, Category = "SK|LockOn")
	float CurrentZoomDistance = 600.f; // 기본값
	//카메라 줌 거리
	UPROPERTY(EditAnywhere, Category = "SK|LockOn")
	float MinCameraZoom = 200.f;
	
	bool IsLockOnReleasing() const;

	// 위치 보간 속도
	float LockOnLocInterpSpeed = 7.f;
	//// 회전 보간 속도
	float LockOnRotInterpSpeed = 9.f;

	//락온이었는가?
	bool bWasLockedOn = false;

	bool bLockOnReleasing = false;



	float LockOnReleaseTime = 0.f;
	float LockOnReleaseDuration = 0.25f;
	float LockOnReleaseAlpha = 0.f;
	// Release 보간 시작/끝
	FVector ReleaseStartLoc;
	FRotator ReleaseStartRot;

	FVector ReleaseEndLoc;
	FRotator ReleaseEndRot;

	// LockOn 보간(기존)
	FVector SmoothedCameraLoc;
	FRotator SmoothedCameraRot;

protected:
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

private:
};
