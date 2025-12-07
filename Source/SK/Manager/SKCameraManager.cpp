// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SKCameraManager.h"

#include "Character/SKPlayerCharacter.h"
#include "Character/AI/SKAICharacterBase.h"
#include "Controller/SKPlayerController.h"

ASKCameraManager::ASKCameraManager()
{
}

void ASKCameraManager::SetbIsLockedOn(bool ArgIsLockedOn)
{
	if (!bIsLockedOn && bIsLockedOn != ArgIsLockedOn)
	{
		Cast<ASKAICharacterBase>(LockedTarget)->SetOverlayMaterial(LockOnOverlayMaterial, fOutLineActiveTime);
	}
	bIsLockedOn = ArgIsLockedOn;
	if (ASKPlayerController* PC = Cast<ASKPlayerController>(GetOwningPlayerController()))
	{
		PC->SetLockOnState(ArgIsLockedOn, LockedTarget);
	}
}

bool ASKCameraManager::ValidateLockOn(AActor* Player)
{
	if (!LockedTarget)
		return false;

	FVector CamLoc = GetCameraLocation();
	FVector PlayerLoc = Player->GetActorLocation();
	FVector TargetLoc = LockedTarget->GetActorLocation();
	TargetLoc.Z += LockOnHeight;

	// 타겟 가려짐 체크
	if (IsTargetObstructed(CamLoc, TargetLoc))
		return false;

	// 거리 체크
	float Dist = FVector::Dist(PlayerLoc, LockedTarget->GetActorLocation());
	if (Dist > MaxLockDistance || Dist < MinLockDistance)
		return false;

	return true;
}

void ASKCameraManager::AdjustCameraDistance(float WheelValue)
{
	float NewDist = CurrentZoomDistance - WheelValue * 50.f; // 50은 감도

	CurrentZoomDistance = FMath::Clamp(NewDist, MinCameraZoom, MaxCameraZoom);
}

void ASKCameraManager::OnTargetChanged(AActor* OldTarget, AActor* NewTarget)
{
	if (IsValid(OldTarget))
	{
		Cast<ASKAICharacterBase>(OldTarget)->ClearOverlayMaterial();
	}

	// 신규 타겟 Overlay 적용
	if (IsValid(NewTarget))
	{
		Cast<ASKAICharacterBase>(NewTarget)->SetOverlayMaterial(LockOnOverlayMaterial, fOutLineActiveTime);
	}
}

void ASKCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (!OutVT.Target)
		return;

	AActor* Player = OutVT.Target;

	if (bIsLockedOn && !ValidateLockOn(Player))
	{
		bIsLockedOn = false;
		LockedTarget = nullptr;
		return;
	}


	FVector PlayerLoc = Player->GetActorLocation();
	FRotator CurrentRot = OutVT.POV.Rotation;

	// Non-LockOn 카메라
	if (!bIsLockedOn)
	{
		FVector DesiredLoc = PlayerLoc - CurrentRot.Vector() * CurrentZoomDistance;
		OutVT.POV.Location = DesiredLoc;
		return;
	}

	// --- LockOn 회전 계산 ---
	FVector TargetLoc = LockedTarget->GetActorLocation();
	FRotator TargetRot = (TargetLoc - PlayerLoc).Rotation();

	FRotator SmoothRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RotateSpeed);

	FVector DesiredLoc = PlayerLoc - SmoothRot.Vector() * CurrentZoomDistance;

	OutVT.POV.Location = DesiredLoc;
	OutVT.POV.Rotation = SmoothRot;
}

bool ASKCameraManager::IsTargetObstructed(const FVector& CamLoc, const FVector& TargetLoc)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(LockedTarget);
	Params.AddIgnoredActor(GetOwningPlayerController()->GetPawn());

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		CamLoc,
		TargetLoc,
		ECC_Visibility,
		Params
	);

	if (!bHit)
		return false;

	return Hit.GetActor() != LockedTarget;
}
