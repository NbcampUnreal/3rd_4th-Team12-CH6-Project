// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SKCameraManager.h"

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


void ASKCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (!OutVT.Target)
		return;

	AActor* Player = OutVT.Target;
	APawn* Pawn = Cast<APawn>(Player);
	if (!Pawn)
		return;

	ASKPlayerController* SKPC = Cast<ASKPlayerController>(PCOwner);
	const bool bLockedOn = SKPC && SKPC->GetIsLockedOn();

	if (!bLockedOn)
		return;

	const float HalfHeight = Pawn->GetSimpleCollisionHalfHeight();

	const float DistanceAlpha = FMath::GetMappedRangeValueClamped(
		FVector2D(150.f, 600.f),
		FVector2D(0.f, 1.f),
		CurrentZoomDistance
	);


	float TargetDistance = FLT_MAX;
	AActor* LockedTarget = nullptr;

	LockedTarget = SKPC->GetLockedTarget();
	if (IsValid(LockedTarget))
	{
		TargetDistance = FVector::Dist(
			Player->GetActorLocation(),
			LockedTarget->GetActorLocation()
		);
	}


	// Target 거리 Alpha (가까울수록 1)
	const float TargetAlpha = FMath::GetMappedRangeValueClamped(
		FVector2D(150.f, 400.f),
		FVector2D(1.f, 0.f),
		TargetDistance
	);


	const float BaseZ = HalfHeight * 0.6f;

	float ZOffset = FMath::Lerp(
		HalfHeight * 0.05f,
		HalfHeight * 0.25f,
		DistanceAlpha
	);

	// 타겟이 가까울수록 Z 보정 완화
	ZOffset *= TargetAlpha;

	FVector CameraBaseLoc = Player->GetActorLocation();
	CameraBaseLoc.Z += BaseZ + ZOffset;

	FRotator CamRot = OutVT.POV.Rotation;

	const FRotator YawOnlyRot(0.f, CamRot.Yaw, 0.f);
	const FVector BackDir = YawOnlyRot.Vector();

	FVector DesiredLoc = CameraBaseLoc - BackDir * CurrentZoomDistance;


	FVector LookAtPoint = LockedTarget->GetActorLocation();
	// LookAtPoint.Z += HalfHeight * 0.5f;
	LookAtPoint.Z += LockOnLookOffsetZ;   

	CamRot = (LookAtPoint - DesiredLoc).Rotation();
	
	const float PitchMin = FMath::Lerp(-40.f, -65.f, TargetAlpha);
	CamRot.Pitch = FMath::Clamp(CamRot.Pitch, PitchMin, 10.f);


	OutVT.POV.Location = DesiredLoc;
	OutVT.POV.Rotation = CamRot;
}


UMaterialInterface* ASKCameraManager::Get_OutLineMat()
{
	return LockOnOverlayMaterial;
}

float ASKCameraManager::Get_OutLineTime()
{
	return fOutLineActiveTime;
}
