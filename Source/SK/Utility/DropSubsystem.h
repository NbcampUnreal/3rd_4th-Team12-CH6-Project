// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Item/Pickup/Data/SKPickupItemData.h"
#include "DropSubsystem.generated.h"

/**
 * 몬스터 드랍시스템
 */
UCLASS()
class SK_API UDropSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	//몬스터가 사망했을 때 호출되는 함수
	UFUNCTION(BlueprintCallable)
	void ProcessDropTable(int32 DropTableID, const FVector& Origin);

private:

	//실제 아이템을 월드에 Spawn 하는 함수.
	void SpawnPickup(int32 ItemID, USKPickupItemData* PickupData, int32 Count, const FVector& Origin);
};
