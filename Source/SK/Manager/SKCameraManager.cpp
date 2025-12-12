// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SKCameraManager.h"

#include "Character/SKPlayerCharacter.h"
#include "Character/AI/SKAICharacterBase.h"
#include "Controller/SKPlayerController.h"
#include "Net/UnrealNetwork.h"

ASKCameraManager::ASKCameraManager()
{
}

void ASKCameraManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}



bool ASKCameraManager::ValidateLockOn(AActor* Player)
{
	// if (!LockedTarget)
	// 	return false;

	// FVector CamLoc = GetCameraLocation();
	// FVector PlayerLoc = Player->GetActorLocation();
	// FVector TargetLoc = LockedTarget->GetActorLocation();
	// TargetLoc.Z += LockOnHeight;
	//
	// // 타겟 가려짐 체크
	// if (IsTargetObstructed(CamLoc, TargetLoc))
	// 	return false;
	//
	// // 거리 체크
	// float Dist = FVector::Dist(PlayerLoc, LockedTarget->GetActorLocation());
	// if (Dist > MaxLockDistance || Dist < MinLockDistance)
	// {
	// 	
	// 	return false;
	// }

	return true;
}

void ASKCameraManager::AdjustCameraDistance(float WheelValue)
{
	float NewDist = CurrentZoomDistance - WheelValue * 50.f; // 50은 감도

	CurrentZoomDistance = FMath::Clamp(NewDist, MinCameraZoom, MaxCameraZoom);
}

UMaterialInterface* ASKCameraManager::Get_OutLineMat()
{
	return LockOnOverlayMaterial;
}

float ASKCameraManager::Get_OutLineTime()
{
	return fOutLineActiveTime;
}


void ASKCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);


}
