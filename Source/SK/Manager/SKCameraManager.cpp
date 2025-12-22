// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SKCameraManager.h"


#include "Camera/CameraShakeBase.h"
#include "Controller/SKPlayerController.h"


ASKCameraManager::ASKCameraManager()
{
}

void ASKCameraManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


bool ASKCameraManager::ValidateLockOn(AActor* Player)
{
	return true;
}

void ASKCameraManager::AdjustCameraDistance(float WheelValue)
{
	float NewDist = CurrentZoomDistance - WheelValue * 50.f; // 50은 감도

	CurrentZoomDistance = FMath::Clamp(NewDist, MinCameraZoom, MaxCameraZoom);
}


bool ASKCameraManager::IsLockOnReleasing() const
{
	return bLockOnReleasing;
}


void ASKCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	ApplyCameraModifiers(DeltaTime, OutVT.POV);
}

UMaterialInterface* ASKCameraManager::Get_OutLineMat()
{
	return LockOnOverlayMaterial;
}

float ASKCameraManager::Get_OutLineTime()
{
	return fOutLineActiveTime;
}
