// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ObjectPool/PoolManager.h"
#include "GameData/ObjectPoolDataAsset.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "ObjectPoolingSubsystem.generated.h"

/**
 * 통합 오브젝트 풀링 시스템
 */
UCLASS()
class SK_API UObjectPoolingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void InitializeFromDataAsset(UObjectPoolDataAsset* ConfigAsset);

	/** 🔹 오브젝트 가져오기 (Actor / Sound / Niagara 자동 식별) */
	template<typename T>
	T* GetObject(UWorld* World, TFunction<T*(UWorld*)> Constructor);

	/** 🔹 오브젝트 반환 */
	template<typename T>
	void ReturnObject(T* Object);

	// 컴퍼넌트 풀용 루트 액터
	UPROPERTY()
	AActor* ComponentRootActor = nullptr;   

protected:
	/** 자동 초기화에 사용할 Pool 설정 DataAsset */
	UPROPERTY(EditAnywhere, Category = "Pooling")
	TObjectPtr<UObjectPoolDataAsset> DefaultConfigAsset;

private:
	PoolManager<AActor> ActorPool;
	PoolManager<UAudioComponent> SoundPool;
	PoolManager<UNiagaraComponent> NiagaraPool;
};

template<typename T>
T* UObjectPoolingSubsystem::GetObject(UWorld* World, TFunction<T*(UWorld*)> Constructor)
{
	if constexpr (TIsDerivedFrom<T, AActor>::IsDerived)
		return ActorPool.GetFromPool(World, Constructor);
	else if constexpr (TIsDerivedFrom<T, UAudioComponent>::IsDerived)
		return SoundPool.GetFromPool(World, Constructor);
	else if constexpr (TIsDerivedFrom<T, UNiagaraComponent>::IsDerived)
		return NiagaraPool.GetFromPool(World, Constructor);

	return nullptr;
}

template<typename T>
void UObjectPoolingSubsystem::ReturnObject(T* Object)
{
	if constexpr (TIsDerivedFrom<T, AActor>::IsDerived)
	{
		ActorPool.ReturnToPool(Object);
	}
	else if constexpr (TIsDerivedFrom<T, UAudioComponent>::IsDerived)
	{
		Object->Stop();
		SoundPool.ReturnToPool(Object);
	}
	else if constexpr (TIsDerivedFrom<T, UNiagaraComponent>::IsDerived)
	{
		Object->DeactivateImmediate();
		NiagaraPool.ReturnToPool(Object);
	}
}
