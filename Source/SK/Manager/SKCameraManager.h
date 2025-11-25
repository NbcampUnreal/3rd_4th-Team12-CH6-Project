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
	
	UPROPERTY(BlueprintReadWrite)
	AActor* LockedTarget;

	UPROPERTY(BlueprintReadWrite)
	bool bIsLockedOn = false;

	// 회전 속도
	UPROPERTY(EditAnywhere,Category = "SK|LockOn")
	float LockOnInterpSpeed = 7.f;

	UPROPERTY(EditAnywhere,Category = "SK|LockOn")
	float LockOnHeight = 60.f;

	UPROPERTY(EditAnywhere,Category = "SK|LockOn")
	float LockOnPitch = 10.f;

	UPROPERTY(EditAnywhere,Category = "SK|LockOn")
	float MaxLockDistance = 2000.f;

	UPROPERTY(EditAnywhere,Category = "SK|LockOn")
	float MinLockDistance = 200.f;
	
protected:
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

private:
	bool IsTargetObstructed(const FVector& CamLoc, const FVector& TargetLoc);
	
};
