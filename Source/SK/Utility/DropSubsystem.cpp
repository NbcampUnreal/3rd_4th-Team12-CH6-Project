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

		SpawnPickup(Item.ItemID, Item.PickupData, Count, Origin);
	}
}

void UDropSubsystem::SpawnPickup(int32 ItemID, USKPickupItemData* PickupData, int32 Count, const FVector& Origin)
{
	if (!PickupData) return;

	FVector Offset(
		FMath::FRandRange(-300.0f, 300.0f),
		FMath::FRandRange(-300.0f, 300.0f),
		0
	);

	FVector StartLoc = Origin + Offset;
	FVector EndLoc = StartLoc - FVector(0, 0, 2000.0f);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(DropTrace), false);
	Params.bReturnPhysicalMaterial = false;
	Params.AddIgnoredActor(nullptr);

	FVector SpawnLoc = StartLoc;

	//지면 히트 성공 → 그 위치에 스폰
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartLoc, EndLoc, ECC_Visibility, Params))
	{
		SpawnLoc = Hit.ImpactPoint + FVector(0, 0, 5.0f); // 살짝 띄워서 안전하게
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ASKPickupItem* DropActor = GetWorld()->SpawnActor<ASKPickupItem>(
			ASKPickupItem::StaticClass(),
		SpawnLoc,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!DropActor) return;

	// SKPickupItem 구조 적용
	DropActor->InitializePickup(ItemID, PickupData, Count);

	UE_LOG(LogTemp, Log, TEXT("[DropSubsystem] Spawned Pickup: %s (%d개)"),
		*PickupData->ItemName.ToString(), Count);
}