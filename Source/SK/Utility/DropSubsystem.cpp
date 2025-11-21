// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/DropSubsystem.h"
#include "Utility/StaticDataSubsystem.h"
#include "GameData/StaticData/DropTableData.h"
#include "Item/Pickup/SKPickupItem.h"

void UDropSubsystem::ProcessDropTable(int32 DropTableID, const FVector& Origin)
{
	UWorld* World = GetWorld();
	if (!World) return;

	//클라이언트에서는 드랍관련 계산 및 스폰 안됨
	if (World->GetNetMode() == NM_Client) return;

	auto* StaticData = World->GetGameInstance()->GetSubsystem<UStaticDataSubsystem>();
	if (!StaticData) return;

	const FDropTableData* Table = StaticData->GetData<FDropTableData>(DropTableID);
	if (!Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DropSubsystem] DropTable not found: %d"), DropTableID);
		return;
	}

	// 각각의 아이템 항목 독립 확률로 굴림
	for (const FDropItemData& Item : Table->Items)
	{
		if (FMath::FRand() > Item.Probability)
			continue;

		int32 Count = FMath::RandRange(Item.MinCount, Item.MaxCount);

		SpawnPickup(Item.PickupData, Count, Origin);
	}
}

void UDropSubsystem::SpawnPickup(USKPickupItemData* PickupData, int32 Count, const FVector& Origin)
{
	if (!PickupData) return;

	FVector Offset(
		FMath::FRandRange(-300.0f, 300.0f),
		FMath::FRandRange(-300.0f, 300.0f),
		FMath::FRandRange(10.0f, 30.0f)
	);

	FVector SpawnLoc = Origin + Offset;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ASKPickupItem* DropActor = GetWorld()->SpawnActor<ASKPickupItem>(
			ASKPickupItem::StaticClass(),
		SpawnLoc,
		FRotator::ZeroRotator,
		Params
	);

	if (!DropActor) return;

	// SKPickupItem 구조 적용
	DropActor->InitializePickup(PickupData, Count);

	UE_LOG(LogTemp, Log, TEXT("[DropSubsystem] Spawned Pickup: %s (%d개)"),
		*PickupData->ItemName.ToString(), Count);
}