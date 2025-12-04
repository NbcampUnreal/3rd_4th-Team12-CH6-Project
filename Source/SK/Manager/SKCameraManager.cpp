// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SKCameraManager.h"

#include "Character/AI/SKAICharacterBase.h"

ASKCameraManager::ASKCameraManager()
{
}

void ASKCameraManager::SetbIsLockedOn(bool ArgIsLockedOn)
{
	if (!bIsLockedOn &&bIsLockedOn != ArgIsLockedOn)
	{
		Cast<ASKAICharacterBase>(LockedTarget)->SetOverlayMaterial(LockOnOverlayMaterial,fOutLineActiveTime);
	}
	bIsLockedOn = ArgIsLockedOn;
	
}

void ASKCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (!bIsLockedOn || !LockedTarget)
		return;

	AActor* Player = OutVT.Target;

	FVector CamLoc = GetCameraLocation(); //카메라위치
	
	FVector TargetLoc = LockedTarget->GetActorLocation(); //타겟위치
	TargetLoc.Z += LockOnHeight;  // 몬스터 높이 보정


	// 타겟 가려짐 체크
	if (IsTargetObstructed(CamLoc, TargetLoc))
	{
		SetbIsLockedOn(false);;
		LockedTarget = nullptr;
		return;
	}

	// 거리 초과 체크
	float Dist = FVector::Dist(Player->GetActorLocation(), LockedTarget->GetActorLocation());
	if (Dist > MaxLockDistance || Dist < MinLockDistance)
	{
		SetbIsLockedOn(false);;
		LockedTarget = nullptr;
		return;
	}

	// 타겟 바라보기
	FRotator TargetRot = (TargetLoc - CamLoc).Rotation();
	TargetRot.Pitch -= LockOnPitch;
	FRotator NewRot = FMath::RInterpTo(GetCameraRotation(), TargetRot, DeltaTime, LockOnInterpSpeed);

	GetOwningPlayerController()->SetControlRotation(NewRot);
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

	if (!bHit) return false;

	return Hit.GetActor() != LockedTarget;
}
