#include "SK_GA_CombatBase.h"

#include "Character/SKPlayerCharacter.h"
#include "Weapon/ActorComponent/SKActionComponent.h"
#include "AbilitySystemComponent.h"
#include "VectorTypes.h"
#include "Character/AI/SKAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "Math/UnitConversion.h"
#include "PlayerState/SKPlayerState.h"

USK_GA_CombatBase::USK_GA_CombatBase()
	: BaseDistance(100.f), DualOffset(30.f), KatanaOffset(70.f), TraceDist(0.f), TraceRadius(60.0f), SnapDist(0.f)
{
}

void USK_GA_CombatBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASKPlayerCharacter* Character = Cast<ASKPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (Character)
	{
		USKActionComponent* ActionComponent = Character->GetActionComponent();
		if (ActionComponent)
		{
			ActionComponent->OnCombatAction(true);
		}
	}
}

void USK_GA_CombatBase::SnapToTarget(ASKPlayerCharacter* SKPlayer)
{
	UMotionWarpingComponent* MW = SKPlayer->GetMotionWarpingComponent();
	if (!MW) return;

	MW->RemoveWarpTarget(TEXT("TargetAI"));

	FVector TargetLoc = GetSnapLocation(SKPlayer);

	if (TargetLoc !=  FVector::ZeroVector)
	{
		MW->AddOrUpdateWarpTargetFromLocation(
			TEXT("TargetAI"),
			TargetLoc
		);
		
	}
}

FVector USK_GA_CombatBase::GetSnapLocation(ASKPlayerCharacter* SKPlayer)
{
	ASKPlayerState* PS = SKPlayer->GetPlayerState<ASKPlayerState>();
	if (!PS) return FVector::ZeroVector;

	float TargetDistance = BaseDistance;

	FGameplayTag WeaponTag = PS->GetWeaponTag();
	if (WeaponTag == FGameplayTag::RequestGameplayTag(TEXT("Weapon.Dual")))
	{
		TargetDistance = BaseDistance + DualOffset;
	}
	else if (WeaponTag == FGameplayTag::RequestGameplayTag(TEXT("Weapon.Katana")))
	{
		TargetDistance = BaseDistance + KatanaOffset;
	}

	FVector Start = SKPlayer->GetActorLocation();
	Start.Z += SKPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.6f;

	// 실제 트레이스 최대 거리 = TargetDistance + TraceDist + TraceRadius
	float TraceDistance = TargetDistance + TraceDist;
	TargetDistance = TargetDistance + SnapDist;
	
	FVector Forward = SKPlayer->GetActorForwardVector();
	FVector End = Start + (Forward * TraceDistance);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(SKPlayer);

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel4);
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);
	
	bool bHit = GetWorld()->SweepSingleByObjectType(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ObjectParams,
		Sphere,
		Params
	);

#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		End,        
		TraceRadius,    
		16,            
		bHit ? FColor::Red : FColor::Green,
		false,             
		1.f                 
	);
#endif
	
	if (bHit && Hit.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit is True"))
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor != SKPlayer)
		{
			ASKAICharacter* AICharacter = Cast<ASKAICharacter>(HitActor);
			if (AICharacter)
			{
				FVector MonsterLoc = HitActor->GetActorLocation();
				FVector CharacterLoc = SKPlayer->GetActorLocation();

				float CurrentDistance = FVector::Dist(CharacterLoc, MonsterLoc);
				if (CurrentDistance <= TargetDistance)
				{
					return FVector::ZeroVector;
				}

				FVector Direction = (CharacterLoc - MonsterLoc).GetSafeNormal();
				FVector WarpLocation = MonsterLoc + Direction * TargetDistance;
				UE_LOG(LogTemp, Warning, TEXT("CharacterLocation: %s"), *CharacterLoc.ToString());
				UE_LOG(LogTemp, Warning, TEXT("MonsterLocation: %s"), *MonsterLoc.ToString());
				UE_LOG(LogTemp, Warning, TEXT("WarpLocation: %s, TargetDistance: %.2f, SnapDistance: %.2f"), *WarpLocation.ToString(), TargetDistance, SnapDist);
				return WarpLocation;
			}
		}
	}
	
	return FVector::ZeroVector;
}