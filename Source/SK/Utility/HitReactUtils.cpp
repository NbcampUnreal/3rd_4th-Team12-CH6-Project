// Fill out your copyright notice in the Description page of Project Settings.


#include "HitReactUtils.h"
#include "GameFramework/Actor.h"

EHitReactAnim DetermineHitReactAnim(
	const FVector& HitSourceLocation,
	const AActor* Victim
)
{
	const FVector VictimLoc = Victim->GetActorLocation();
	const FVector HitDir = (HitSourceLocation - VictimLoc).GetSafeNormal();

	const FVector Forward = Victim->GetActorForwardVector();
	const FVector Right   = Victim->GetActorRightVector();

	const float ForwardDot = FVector::DotProduct(Forward, HitDir);
	const float RightDot   = FVector::DotProduct(Right, HitDir);

	if (ForwardDot < 0.f)
	{
		return EHitReactAnim::Back;
	}

	return (RightDot >= 0.f)
		? EHitReactAnim::FrontRight
		: EHitReactAnim::FrontLeft;
}