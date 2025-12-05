// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/EquipmentInstance.h"

#include "GameFramework/Character.h"
#include "Item/Inventory/Data/SKEquipmentItemData.h"
#include "Net/UnrealNetwork.h"

void UEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentInstance, GrantedAbilityHandle);
	DOREPLIFETIME(UEquipmentInstance, GrantedEffectHandle);
	DOREPLIFETIME(UEquipmentInstance, SpawnedActors);
	DOREPLIFETIME(UEquipmentInstance, EquipTag);
}

void UEquipmentInstance::SpawnEquipmentActors(APawn* OwningPawn, const TArray<FSKEquipmentActorToSpawn>& ActorsToSpawn)
{
	if (!OwningPawn || ActorsToSpawn.Num() == 0)
		return;

	USceneComponent* AttachTarget = OwningPawn->GetRootComponent();

	if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
	{
		AttachTarget = Char->GetMesh();
	}

	for (const FSKEquipmentActorToSpawn& SpawnData : ActorsToSpawn)
	{
		if (!SpawnData.ActorToSpawn)
			continue;

		AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(
			SpawnData.ActorToSpawn,
			FTransform::Identity,
			OwningPawn
		);

		if (NewActor)
		{
			NewActor->FinishSpawning(FTransform::Identity, true);

			// Attach
			NewActor->AttachToComponent(
				AttachTarget,
				FAttachmentTransformRules::KeepRelativeTransform,
				SpawnData.AttachSocket
			);

			// RelativeTransform 덮어쓰기
			NewActor->SetActorRelativeTransform(SpawnData.AttachTransform);

			SpawnedActors.Add(NewActor);
		}
	}
}

void UEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}

	SpawnedActors.Empty();
}
