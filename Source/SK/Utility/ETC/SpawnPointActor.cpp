// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/ETC/SpawnPointActor.h"
#include "Utility/SpawnSubsystem.h"

// Sets default values
ASpawnPointActor::ASpawnPointActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ASpawnPointActor::BuildSpawnPointData(FSpawnPointData& OutData) const
{
	UWorld* World = GetWorld();
	if (!World) return;

	USpawnSubsystem* SpawnSubsystem = World->GetSubsystem<USpawnSubsystem>();
	if (!SpawnSubsystem) return;

	TArray<FSpawnPointData> Data;
	//기존 SpawnPointData 구조 그대로 사용	
	Data[0].Location = GetActorLocation();
	Data[0].SpawnRuleID = SpawnRuleID;
	Data[0].GroupID = GroupID;
	Data[0].MaxSpawnCount = MaxSpawnCount;

	SpawnSubsystem->RegisterSpawnPoints(Data);
}

void ASpawnPointActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World)
		return;

	USpawnSubsystem* SpawnSubsystem =
		World->GetSubsystem<USpawnSubsystem>();

	if (!SpawnSubsystem)
		return;

	FSpawnPointData Data;
	Data.Location = GetActorLocation();
	Data.SpawnRuleID = SpawnRuleID;
	Data.GroupID = GroupID;
	Data.MaxSpawnCount = MaxSpawnCount;

	TArray<FSpawnPointData> DataArray;
	DataArray.Add(Data);

	SpawnSubsystem->RegisterSpawnPoints(DataArray);
}
