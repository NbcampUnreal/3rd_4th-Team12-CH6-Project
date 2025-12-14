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
	if (!SKPC->GetIsLockedOn())
	{
		return;
	}

	AActor* LockedTarget = SKPC->GetLockedTarget();
	if (!IsValid(LockedTarget))
		return;

	const float HalfHeight = Pawn->GetSimpleCollisionHalfHeight();


	FVector PlayerCenter = Player->GetActorLocation();
	PlayerCenter.Z += HalfHeight * 0.6f;


	FVector TargetLookAt = LockedTarget->GetActorLocation();
	TargetLookAt.Z += HalfHeight * 0.5f;


	const float AngleRad = FMath::DegreesToRadians(LockOnAngleDeg);

	const float HorizontalDist = CurrentZoomDistance * FMath::Cos(AngleRad);
	const float VerticalDist = CurrentZoomDistance * FMath::Sin(AngleRad);

	const FRotator PrevRot = OutVT.POV.Rotation;
	const FRotator YawOnlyRot(0.f, PrevRot.Yaw, 0.f);
	const FVector BackDir = YawOnlyRot.Vector();

	FVector CameraLoc =
		PlayerCenter
		- BackDir * HorizontalDist
		+ FVector::UpVector * VerticalDist;


	FRotator CamRot = (TargetLookAt - CameraLoc).Rotation();

	// Pitch 제한
	CamRot.Pitch = FMath::Clamp(CamRot.Pitch, -70.f, -10.f);


	OutVT.POV.Location = CameraLoc;
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
