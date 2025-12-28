// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitReactUtils.generated.h"

class AActor;

UENUM(BlueprintType)
enum class EHitReactAnim : uint8
{
	FrontLeft  UMETA(DisplayName = "FrontLeft"),
	FrontRight UMETA(DisplayName = "FrontRight"),
	Back       UMETA(DisplayName = "Back"),
};

UENUM(BlueprintType)
enum class EHitReactType : uint8
{
	Normal,
	Heavy,
	Unblockable
};

EHitReactAnim DetermineHitReactAnim(
	const FVector& HitSourceLocation,
	const AActor* Victim
);