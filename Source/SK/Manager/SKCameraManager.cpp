// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SKCameraManager.h"

#include "Character/SKPlayerCharacter.h"
#include "Character/AI/SKAICharacterBase.h"
#include "Net/UnrealNetwork.h"

ASKCameraManager::ASKCameraManager()
{
}

void ASKCameraManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASKCameraManager, bIsLockedOn);
	
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

void ASKCameraManager::SetLockedTarget(AActor* NewTarget)
{
	OldTarget = LockedTarget;
	LockedTarget = NewTarget;

	if (OldTarget != NewTarget)
	{
		OnTargetChanged(NewTarget);
	}

	//  널이 아니면 트루 
	bool result = (NewTarget != nullptr);
	SetIsLockedOn(result);
}

void ASKCameraManager::OnTargetChanged(AActor* NewTarget)
{
	UE_LOG(LogTemp, Warning, TEXT("[Cam] Clearing Overlay Start"));
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

bool ASKCameraManager::GetIsLockedOn()
{
	return bIsLockedOn;
}

void ASKCameraManager::SetIsLockedOn(bool ArgIsLockedOn)
{
	bIsLockedOn = ArgIsLockedOn;
	Camera_SetLockOnState();
}

void ASKCameraManager::OnRep_OnLockOnChange()
{

	Camera_SetLockOnState();
}

void ASKCameraManager::Camera_SetLockOnState()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
		return;

	ASKPlayerCharacter* Player = Cast<ASKPlayerCharacter>(PC->GetPawn());
	if (!Player)
		return;
	Player->PlayAnim_SetLockOnState();
}


void ASKCameraManager::Server_SetLockOnState_Implementation(bool bLock)
{

	Camera_SetLockOnState();
	SetIsLockedOn(bLock);
}

void ASKCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (!OutVT.Target)
		return;

	AActor* Player = OutVT.Target;
	
	if (GetIsLockedOn() && !ValidateLockOn(Player))
	{
		SetIsLockedOn(false);
		LockedTarget = nullptr;
		return;
	}

	FVector PlayerLoc = Player->GetActorLocation();
	FRotator CurrentRot  = OutVT.POV.Rotation; 

	if (!bIsLockedOn)
	{
		FVector DesiredLoc = PlayerLoc - CurrentRot.Vector() * CurrentZoomDistance;
		OutVT.POV.Location = DesiredLoc;
		return;
	}

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
	Params.AddIgnoredActor(GetOwningPlayerController()->GetPawn());
	Params.AddIgnoredActor(LockedTarget);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		CamLoc,
		TargetLoc,
		ECC_Visibility,
		Params
	);
	return (bHit && Hit.GetActor() != LockedTarget);
	// if (!bHit)
	// 	return false;
	//
	// return Hit.GetActor() != LockedTarget;
}
