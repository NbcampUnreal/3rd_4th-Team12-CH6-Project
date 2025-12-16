#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameData/SpawnPointData.h"
#include "SpawnSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SK_API USpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** PCG가 생성한 스폰 포인트 등록 */
	UFUNCTION(BlueprintCallable)
	void RegisterSpawnPoints(const TArray<FSpawnPointData>& Points);

	/** MonsterID로 직접 스폰 */
	AActor* SpawnMonsterByID(int32 MonsterID, const FVector& Location);

	/** RuleID 기반으로 스폰 (몬스터 종류 + 개수 모두 자동 처리) */
	UFUNCTION(BlueprintCallable)
	void SpawnByRuleID(int32 RuleID);

	/** GroupID 단위 리스폰 */
	UFUNCTION(BlueprintCallable)
	void RespawnGroup(int32 GroupID);

	/** 초기 전체 스폰 */
	UFUNCTION(BlueprintCallable)
	void SpawnAll();

	/** 전체 리스폰 */
	UFUNCTION(BlueprintCallable)
	void RespawnAll();

private:
	UWorld* GetWorldChecked() const;

	/** 모든 포인트 데이터 */
	TArray<FSpawnPointData> SpawnPoints;

	/** 정적 데이터에서 룰 가져오기 */
	const struct FMonsterSpawnRule* GetRule(int32 RuleID) const;

	/** 가중치 랜덤 선택 */
	int32 PickMonsterID(const struct FMonsterSpawnRule* Rule) const;

	float SpawnScatterRadius = 500.f;
};
