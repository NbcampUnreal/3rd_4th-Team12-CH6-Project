#include "Anim/AI/SKAnimNotifyState.h"
#include "Kismet/KismetSystemLibrary.h"

USKAnimNotifyState::USKAnimNotifyState()
{
	
}

void USKAnimNotifyState::NotifyBegin(
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

	PrevSocketLocations.Empty();
	SocketHitActors.Empty();

	for (const FName& SocketName : SocketNames)
	{
		PrevSocketLocations.Add(SocketName, MeshComp->GetSocketLocation(SocketName));
		SocketHitActors.Add(SocketName);
	}

	if (ObjectTypes.Num() == 0)
	{
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	}
}

void USKAnimNotifyState::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
	)
{
	PrevSocketLocations.Empty();
	SocketHitActors.Empty();
	
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

void USKAnimNotifyState::NotifyTick(
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
		
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(Owner);

		// 이미 맞은 액터는 제외
		for (const auto& WeakActor : SocketHitActors[Socket])
		{
			if (WeakActor.IsValid())
				IgnoreActors.Add(WeakActor.Get());
		}

		TArray<FHitResult> Hits;

		// 프레임 사이 스윕
		const bool bHit = UKismetSystemLibrary::CapsuleTraceMultiForObjects(
			MeshComp,
			Prev,
			Curr,
			CapsuleRadius,
			CapsuleHalfHeight,
			ObjectTypes,
			false,
			IgnoreActors,
			EDrawDebugTrace::ForDuration,
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
				if (!HitActor || HitActor == Owner)
					continue;

				if (SocketHitActors[Socket].Contains(HitActor))
					continue;

				SocketHitActors[Socket].Add(HitActor);

				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("맞았음"));
			}
		}

		// 현재 위치를 다음 프레임 기준점으로
		PrevSocketLocations[Socket] = Curr;
	}
}
