// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/SpawnSubsystem.h"
#include "Utility/StaticDataSubsystem.h"
#include "GameData/StaticData/MonsterDataTable.h"
#include "GameData/StaticData/MonsterSpawnRule.h"
#include "GameFramework/Actor.h"
#include "GameMode/DungeonGameMode.h"
#include "Character/AI/SKAICharacter.h"
#include "Components/CapsuleComponent.h"


void USpawnSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void USpawnSubsystem::Deinitialize()
{
    Super::Deinitialize();
    SpawnPoints.Empty();
}

void USpawnSubsystem::RegisterSpawnPoints(const TArray<FSpawnPointData>& Points)
{
    SpawnPoints.Append(Points);
}

UWorld* USpawnSubsystem::GetWorldChecked() const
{
    UWorld* World = GetWorld();
    check(World);
    return World;
}

/**
 * MonsterID로 스폰하며, 필요하면 지형에 맞춰 위치 보정까지 수행
 */
AActor* USpawnSubsystem::SpawnMonsterByID(int32 MonsterID, const FVector& InLocation)
{
    if (GetWorldChecked()->GetNetMode() == NM_Client) return nullptr;

    // 1) 정적 데이터에서 몬스터 데이터 가져오기
    auto* SDS = GetWorld()->GetGameInstance()->GetSubsystem<UStaticDataSubsystem>();
    const FMonsterData* Data = SDS->GetData<FMonsterData>(MonsterID);
    if (!Data) return nullptr;

    // 2) 스폰 클래스 로드
    TSubclassOf<AActor> ClassToSpawn = Data->MonsterClass.LoadSynchronous();
    if (!ClassToSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("[SpawnSubSystem] Not TSubclassOf MonsterClass"));
        return nullptr;
    }


    ASKAICharacter* CDO = Cast<ASKAICharacter>(ClassToSpawn->GetDefaultObject());
    if (!CDO)
    {
        UE_LOG(LogTemp, Error, TEXT("[SpawnSubSystem] Not Cast ASKAICharacter Monster"));
        return nullptr;
    }

    // 3) 스폰 몬스터 캡슐 크기 가져오기
    UCapsuleComponent* Capsule = CDO->GetCapsuleComponent();
    if (!Capsule)
    {
        UE_LOG(LogTemp, Error, TEXT("[SpawnSubSystem] Not Monster Capsule"));
        return nullptr;
    }

    float CapsuleRadius = Capsule->GetUnscaledCapsuleRadius();
    float CapsuleHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();

    // 4) 바닥 보정 + 캡슐 높이 반영
    FVector SpawnLocation = InLocation;
    FHitResult Hit;
    FVector Start = InLocation + FVector(0, 0, 1000.f);
    FVector End   = InLocation - FVector(0, 0, 2000.f);
    
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic))
    {
        SpawnLocation.Z = Hit.Location.Z  + CapsuleHalfHeight;
    }

    // 5) 스폰 파라미터 설정 후 SpawnActor
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    return GetWorld()->SpawnActor<AActor>(
        ClassToSpawn,
        SpawnLocation,
        FRotator::ZeroRotator,
        Params
    );
}

/** 룰 데이터 가져오기 */
const FMonsterSpawnRule* USpawnSubsystem::GetRule(int32 RuleID) const
{
    auto* SDS = GetWorldChecked()->GetGameInstance()->GetSubsystem<UStaticDataSubsystem>();
    return SDS->GetData<FMonsterSpawnRule>(RuleID);
}

/** RuleID 기반 스폰 (몬스터 종류 + 개수 모두 처리) */
void USpawnSubsystem::SpawnByRuleID(int32 RuleID)
{
    const FMonsterSpawnRule* Rule = GetRule(RuleID);
    if (!Rule || Rule->SpawnEntries.Num() == 0) return;

    UE_LOG(LogTemp, Warning, TEXT("SpawnPoints Num : %d" ), SpawnPoints.Num());

    for (FSpawnPointData& P : SpawnPoints)
    {
        if (P.SpawnRuleID != RuleID)
            continue;

        // 몬스터 후보들 전체 순회
        for (const FMonsterSpawnEntry& Entry : Rule->SpawnEntries)
        {
            UE_LOG(LogTemp, Warning, TEXT("Spawn MonsterID : %d" ), Entry.MonsterID);
            const int32 SpawnCount = FMath::RandRange(Entry.MinCount, Entry.MaxCount);

            // 개수만큼 스폰
            for (int32 i = 0; i < SpawnCount; i++)
            {
                float R = FMath::RandRange(0.f, SpawnScatterRadius);
                float Theta = FMath::RandRange(0.f, PI * 2);

                FVector Offset(
                    FMath::Cos(Theta) * R,
                    FMath::Sin(Theta) * R,
                    0.f
                );

                FVector SpawnLocation = P.Location + Offset;

                // ② 지면 보정 (SpawnMonsterByID 안에서 다시 보정됨)
                AActor* Spawned = SpawnMonsterByID(Entry.MonsterID, SpawnLocation);

                if (Spawned)
                    P.SpawnedActors.Add(Spawned);
            }
        }
    }
}

/** GroupID 단위 리스폰 */
void USpawnSubsystem::RespawnGroup(int32 GroupID)
{
    for (FSpawnPointData& P : SpawnPoints)
    {
        if (P.GroupID != GroupID)
            continue;

        // 기존 몬스터 제거
        for (auto& ActorPtr : P.SpawnedActors)
        {
            if (AActor* A = ActorPtr.Get())
                A->Destroy();
        }
        P.SpawnedActors.Empty();

        // 스폰 규칙 기반 재스폰
        SpawnByRuleID(P.SpawnRuleID);
    }
}
 
void USpawnSubsystem::SpawnAll()
{
    UWorld* World = GetWorldChecked();
    if (World->GetNetMode() == NM_Client)
        return;

    // 1) SpawnPoints에서 등장하는 모든 RuleID를 수집
    TSet<int32> RuleIDSet;
    for (const FSpawnPointData& P : SpawnPoints)
    {
        if (P.SpawnRuleID > 0)
            RuleIDSet.Add(P.SpawnRuleID);
    }

    if (RuleIDSet.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SpawnSubsystem] SpawnAll(): No RuleID found."));
        return;
    }

    // 2) RuleIDSet을 있는 그대로 순회
    for (int32 RuleID : RuleIDSet)
    {
        SpawnByRuleID(RuleID);
    }

    // GameMode에 초기 스폰 끝났다고 알림
    if (auto* GM = GetWorld()->GetAuthGameMode<ADungeonGameMode>())
    {
        GM->NotifyInitialSpawnFinished();
    }
    UE_LOG(LogTemp, Log, TEXT("[SpawnSubsystem] SpawnAll(): Spawn completed (%d RuleIDs)."), RuleIDSet.Num());
}

void USpawnSubsystem::RespawnAll()
{
    UWorld* World = GetWorldChecked();
    if (World->GetNetMode() == NM_Client) return;
    
    // 1) 기존 몬스터 모두 제거
    for (FSpawnPointData& P : SpawnPoints)
    {
        for (auto& ActorPtr : P.SpawnedActors)
        {
            if (AActor* A = ActorPtr.Get())
            {
                A->Destroy();
            }
        }
        P.SpawnedActors.Empty();
    }

    // 2) SpawnPoints에 있는 RuleID 기준으로 다시 전체 스폰
    TSet<int32> RuleIDSet;
    for (const FSpawnPointData& P : SpawnPoints)
    {
        if (P.SpawnRuleID > 0)
        {
            RuleIDSet.Add(P.SpawnRuleID);
        }
    }

    for (int32 RuleID : RuleIDSet)
    {
        SpawnByRuleID(RuleID);
    }

    UE_LOG(LogTemp, Log, TEXT("[SpawnSubsystem] RespawnAll(): All monsters respawned."));
}
