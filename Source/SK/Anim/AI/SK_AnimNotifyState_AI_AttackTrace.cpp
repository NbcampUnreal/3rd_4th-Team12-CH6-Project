#include "Anim/AI/SK_AnimNotifyState_AI_AttackTrace.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

USK_AnimNotifyState_AI_AttackTrace::USK_AnimNotifyState_AI_AttackTrace()
{
	
}

void USK_AnimNotifyState_AI_AttackTrace::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference
	)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	PrevSocketLocations.Empty();
	IgnoreActors.Empty();
	HitActors.Empty();
	
	IgnoreActors.Add(Owner);
	
	for (const FName& SocketName : SocketNames)
	{
		PrevSocketLocations.Add(SocketName, MeshComp->GetSocketLocation(SocketName));
	}

	if (ObjectTypes.Num() == 0)
	{
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));
	}
}

void USK_AnimNotifyState_AI_AttackTrace::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
	)
{
	PrevSocketLocations.Empty();
	IgnoreActors.Empty();
	HitActors.Empty();

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

void USK_AnimNotifyState_AI_AttackTrace::NotifyTick(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference
	)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	UWorld* World = MeshComp->GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	for (const FName& Socket : SocketNames)
	{
		if (!PrevSocketLocations.Contains(Socket))
			continue;

		const FVector Prev = PrevSocketLocations[Socket];
		const FVector Curr = MeshComp->GetSocketLocation(Socket);

		TArray<FHitResult> Hits;

		const bool bHit = UKismetSystemLibrary::CapsuleTraceMultiForObjects(
			MeshComp,
			Prev,
			Curr,
			CapsuleRadius,
			CapsuleHalfHeight,
			ObjectTypes,
			false,
			IgnoreActors,
			// EDrawDebugTrace::ForDuration,
			EDrawDebugTrace::None,
			////EDrawDebugTrace::ForDuration,
			Hits,
			true,
			FColor::Red,
			FColor::Green,
			0.5f
		);

		if (bHit)
		{
			for (const FHitResult& Hit : Hits)
			{
				AActor* HitActor = Hit.GetActor();
				if (!IsValid(HitActor))
				{
					continue;
				}

				if (HitActors.Contains(HitActor))
				{
					continue;
				}
				
				//FString Msg = FString::Printf(TEXT("Hit 개수: %d"), Hits.Num());
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Msg);
				HitActors.Add(HitActor);
				
				UAbilitySystemComponent* OwnerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
				if (!OwnerASC)
				{
					continue;
				}

				FGameplayEventData EventData;
				EventData.Instigator = Owner;
				EventData.Target = HitActor;
				EventData.EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Hit"));
				EventData.OptionalObject = nullptr;

				OwnerASC->HandleGameplayEvent(EventData.EventTag, &EventData);
			}
		}

		PrevSocketLocations[Socket] = Curr;
	}
}
