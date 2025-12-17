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

void ASKCameraManager::BeginLockOnReleaseFromCurrentView()
{
	if (bLockOnReleasing)
		return;

	bLockOnReleasing = true;
	LockOnReleaseTime = 0.f;

	ReleaseStartLoc = SmoothedCameraLoc;
	ReleaseStartRot = SmoothedCameraRot;

	// End = 자유 카메라 목표
	LerpLockOnOff(ReleaseEndLoc, ReleaseEndRot);

	ReleaseEndRot.Pitch = FMath::Clamp(ReleaseEndRot.Pitch, -70.f, -10.f);
}

bool ASKCameraManager::IsLockOnReleasing() const
{
	return bLockOnReleasing;
}

void ASKCameraManager::CalcFreeCameraTarget(const FTViewTarget& OutVT, FVector& OutLoc, FRotator& OutRot)
{
	AActor* Player = OutVT.Target;
	if (!Player)
		return;

	APawn* Pawn = Cast<APawn>(Player);
	float HalfHeight = Pawn ? Pawn->GetSimpleCollisionHalfHeight() : 0.f;

	FVector PlayerCenter = Player->GetActorLocation();
	PlayerCenter.Z += HalfHeight * 0.6f;

	FRotator ControlRot = PCOwner->GetControlRotation();
	FRotator YawOnly(0.f, ControlRot.Yaw, 0.f);

	const float AngleRad = FMath::DegreesToRadians(LockOnAngleDeg);
	const float HorizontalDist = CurrentZoomDistance * FMath::Cos(AngleRad);
	const float VerticalDist = CurrentZoomDistance * FMath::Sin(AngleRad);

	FVector BackDir = YawOnly.Vector();

	OutLoc =
		PlayerCenter
		- BackDir * HorizontalDist
		+ FVector::UpVector * VerticalDist;

	OutRot = ControlRot;
	OutRot.Pitch = FMath::Clamp(OutRot.Pitch, -70.f, -10.f);
}


void ASKCameraManager::BeginLockOnRelease(const FTViewTarget& OutVT)
{
	bLockOnReleasing = true;
	LockOnReleaseTime = 0.f;

	// Start = 지금 화면
	ReleaseStartLoc = OutVT.POV.Location;
	ReleaseStartRot = OutVT.POV.Rotation;

	// End = 자유 카메라의 목표 상태(예시)
	if (ASKPlayerController* SKPC = Cast<ASKPlayerController>(PCOwner))
	{
		ReleaseEndRot = SKPC->GetControlRotation();
	}
	else
	{
		ReleaseEndRot = ReleaseStartRot;
	}

	ReleaseEndLoc = ReleaseStartLoc;
}

void ASKCameraManager::LerpLockOnOff(FVector& OutLoc, FRotator& OutRot)
{
	APawn* Pawn = PCOwner ? PCOwner->GetPawn() : nullptr;
	if (!Pawn)
	{
		OutLoc = GetCameraLocation();
		OutRot = GetCameraRotation();
		return;
	}

	const float HalfHeight = Pawn->GetSimpleCollisionHalfHeight();

	FVector PlayerCenter = Pawn->GetActorLocation();
	PlayerCenter.Z += HalfHeight * 0.6f;

	FRotator ControlRot = PCOwner->GetControlRotation();
	FRotator YawOnly(0.f, ControlRot.Yaw, 0.f);

	const float AngleRad = FMath::DegreesToRadians(LockOnAngleDeg);
	const float HorizontalDist = CurrentZoomDistance * FMath::Cos(AngleRad);
	const float VerticalDist = CurrentZoomDistance * FMath::Sin(AngleRad);

	FVector BackDir = YawOnly.Vector();

	OutLoc =
		PlayerCenter
		- BackDir * HorizontalDist
		+ FVector::UpVector * VerticalDist;

	OutRot = ControlRot;
	OutRot.Pitch = FMath::Clamp(OutRot.Pitch, -70.f, -10.f);
}

void ASKCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	 Super::UpdateViewTarget(OutVT, DeltaTime);

	// if (!OutVT.Target)
	// 	return;
	//
	// ASKPlayerController* SKPC = Cast<ASKPlayerController>(PCOwner);
	// if (!SKPC)
	// 	return;
	//
	// /* =====================================================
	//    1️⃣ LOCK-ON RELEASE (최우선)
	//    ===================================================== */
	// if (bLockOnReleasing)
	// {
	// 	LockOnReleaseTime += DeltaTime;
	// 	float Alpha = FMath::Clamp(
	// 		LockOnReleaseTime / LockOnReleaseDuration,
	// 		0.f, 1.f
	// 	);
	//
	//
	// 	FVector FreeLoc;
	// 	FRotator FreeRot;
	// 	LerpLockOnOff(FreeLoc, FreeRot);
	//
	//
	// 	FVector CurLoc = FMath::Lerp(
	// 		ReleaseStartLoc,
	// 		FreeLoc,
	// 		Alpha
	// 	);
	//
	// 	FRotator CurRot = FMath::Lerp(
	// 		ReleaseStartRot,
	// 		FreeRot,
	// 		Alpha
	// 	);
	//
	// 	OutVT.POV.Location = CurLoc;
	// 	OutVT.POV.Rotation = CurRot;
	//
	// 	if (Alpha >= 1.f)
	// 	{
	// 		// 자유 카메라로 완전 전환
	// 		SmoothedCameraLoc = CurLoc;
	// 		SmoothedCameraRot = CurRot;
	// 		bLockOnReleasing = false;
	// 	}
	// 	return;
	// }
	//
	// Super::UpdateViewTarget(OutVT, DeltaTime);
	//
	// /* =====================================================
	//    2️⃣ LOCK-ON ACTIVE (기존 로직 유지)
	//    ===================================================== */
	// if (SKPC->GetIsLockedOn())
	// {
	// 	AActor* Player = OutVT.Target;
	// 	APawn* Pawn = Cast<APawn>(Player);
	// 	if (!Pawn)
	// 		return;
	//
	// 	if (!bWasLockedOn)
	// 	{
	// 		SmoothedCameraLoc = OutVT.POV.Location;
	// 		SmoothedCameraRot = OutVT.POV.Rotation;
	// 	}
	// 	bWasLockedOn = true;
	//
	// 	AActor* LockedTarget = SKPC->GetLockedTarget();
	// 	if (!IsValid(LockedTarget))
	// 		return;
	//
	// 	const float HalfHeight = Pawn->GetSimpleCollisionHalfHeight();
	//
	// 	FVector PlayerCenter = Player->GetActorLocation();
	// 	PlayerCenter.Z += HalfHeight * 0.6f;
	//
	// 	FVector TargetLookAt = LockedTarget->GetActorLocation();
	// 	TargetLookAt.Z += HalfHeight * 0.5f;
	//
	// 	const float AngleRad = FMath::DegreesToRadians(LockOnAngleDeg);
	// 	const float HorizontalDist = CurrentZoomDistance * FMath::Cos(AngleRad);
	// 	const float VerticalDist = CurrentZoomDistance * FMath::Sin(AngleRad);
	//
	// 	const FRotator YawOnlyRot(0.f, SmoothedCameraRot.Yaw, 0.f);
	// 	const FVector BackDir = YawOnlyRot.Vector();
	//
	// 	FVector DesiredCameraLoc =
	// 		PlayerCenter
	// 		- BackDir * HorizontalDist
	// 		+ FVector::UpVector * VerticalDist;
	//
	// 	// 충돌 보정
	// 	FVector FinalCameraLoc = DesiredCameraLoc;
	//
	// 	FHitResult HitResult;
	// 	FCollisionQueryParams Params;
	// 	Params.AddIgnoredActor(Player);
	// 	Params.AddIgnoredActor(LockedTarget);
	//
	// 	const float CameraRadius = 12.f;
	//
	// 	if (GetWorld()->SweepSingleByChannel(
	// 		HitResult,
	// 		PlayerCenter,
	// 		DesiredCameraLoc,
	// 		FQuat::Identity,
	// 		ECC_Camera,
	// 		FCollisionShape::MakeSphere(CameraRadius),
	// 		Params))
	// 	{
	// 		FinalCameraLoc = HitResult.Location + HitResult.ImpactNormal * 8.f;
	// 	}
	//
	// 	FRotator CamRot = (TargetLookAt - FinalCameraLoc).Rotation();
	//
	// 	SmoothedCameraLoc = FMath::VInterpTo(
	// 		SmoothedCameraLoc,
	// 		FinalCameraLoc,
	// 		DeltaTime,
	// 		LockOnLocInterpSpeed
	// 	);
	//
	// 	SmoothedCameraRot = FMath::RInterpTo(
	// 		SmoothedCameraRot,
	// 		CamRot,
	// 		DeltaTime,
	// 		LockOnRotInterpSpeed
	// 	);
	//
	// 	SmoothedCameraRot.Pitch = FMath::Clamp(
	// 		SmoothedCameraRot.Pitch,
	// 		-70.f, -10.f
	// 	);
	//
	// 	OutVT.POV.Location = SmoothedCameraLoc;
	// 	OutVT.POV.Rotation = SmoothedCameraRot;
	// 	return;
	// }
	//
	//
	// bWasLockedOn = false;
}



// void ASKCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
// {
// 	Super::UpdateViewTarget(OutVT, DeltaTime);
//
// 	if (!OutVT.Target)
// 		return;
//
// 	AActor* Player = OutVT.Target;
// 	APawn* Pawn = Cast<APawn>(Player);
// 	if (!Pawn)
// 		return;
//
// 	ASKPlayerController* SKPC = Cast<ASKPlayerController>(PCOwner);
//
// 	bool bIsLockedOn = SKPC->GetIsLockedOn();
// 	if (!bIsLockedOn)
// 	{
// 		if (bWasLockedOn)
// 		{
// 			SmoothedCameraLoc = OutVT.POV.Location;
// 			SmoothedCameraRot = OutVT.POV.Rotation;
// 		}
//
//
// 		bWasLockedOn = false;
//
// 		return;
// 	}
//
//
//
// 	if (!bWasLockedOn)
// 	{
// 		SmoothedCameraLoc = OutVT.POV.Location;
// 		SmoothedCameraRot = OutVT.POV.Rotation;
// 	}
// 	bWasLockedOn = true;
//
// 	AActor* LockedTarget = SKPC->GetLockedTarget();
// 	if (!IsValid(LockedTarget))
// 		return;
//
// 	const float HalfHeight = Pawn->GetSimpleCollisionHalfHeight();
//
//
// 	FVector PlayerCenter = Player->GetActorLocation();
// 	PlayerCenter.Z += HalfHeight * 0.6f;
//
//
// 	FVector TargetLookAt = LockedTarget->GetActorLocation();
// 	TargetLookAt.Z += HalfHeight * 0.5f;
//
//
// 	const float AngleRad = FMath::DegreesToRadians(LockOnAngleDeg);
//
// 	const float HorizontalDist = CurrentZoomDistance * FMath::Cos(AngleRad);
// 	const float VerticalDist = CurrentZoomDistance * FMath::Sin(AngleRad);
//
// 	const FRotator PrevRot = OutVT.POV.Rotation;
// 	const FRotator YawOnlyRot(0.f, PrevRot.Yaw, 0.f);
// 	const FVector BackDir = YawOnlyRot.Vector();
// 	
// 	FVector DesiredCameraLoc =
// 		PlayerCenter
// 		- BackDir * HorizontalDist
// 		+ FVector::UpVector * VerticalDist;
//
//
// 	//충돌보정
// 	FVector FinalCameraLoc = DesiredCameraLoc;
//
// 	FHitResult HitResult;
// 	FCollisionQueryParams Params;
// 	Params.AddIgnoredActor(Player);
// 	Params.AddIgnoredActor(LockedTarget);
//
// 	const float CameraRadius = 12.f; // 
//
// 	bool bHit = GetWorld()->SweepSingleByChannel(
// 		HitResult,
// 		PlayerCenter,
// 		DesiredCameraLoc,
// 		FQuat::Identity,
// 		ECC_Camera,
// 		FCollisionShape::MakeSphere(CameraRadius),
// 		Params
// 	);
//
// 	if (bHit)
// 	{
// 		// 충돌 지점에서 살짝 앞으로 당김
// 		FinalCameraLoc = HitResult.Location + HitResult.ImpactNormal * 8.f;
// 	}
//
// 	FRotator CamRot = (TargetLookAt - FinalCameraLoc).Rotation();
//
// 	// // Pitch 제한
// 	// CamRot.Pitch = FMath::Clamp(CamRot.Pitch, -70.f, -10.f);
//
//
// 	SmoothedCameraLoc = FMath::VInterpTo(
// 		SmoothedCameraLoc,
// 		FinalCameraLoc,
// 		DeltaTime,
// 		LockOnLocInterpSpeed // 위치 보간 속도
// 	);
//
// 	SmoothedCameraRot = FMath::RInterpTo(
// 		SmoothedCameraRot,
// 		CamRot,
// 		DeltaTime,
// 		LockOnRotInterpSpeed // 회전 보간 속도
// 	);
//
// 	// Pitch 제한
// 	SmoothedCameraRot.Pitch = FMath::Clamp(CamRot.Pitch, -70.f, -10.f);
//
//
//
// 	OutVT.POV.Location = SmoothedCameraLoc;
// 	OutVT.POV.Rotation = SmoothedCameraRot;
// 	
// }
UMaterialInterface* ASKCameraManager::Get_OutLineMat()
{
	return LockOnOverlayMaterial;
}

float ASKCameraManager::Get_OutLineTime()
{
	return fOutLineActiveTime;
}
