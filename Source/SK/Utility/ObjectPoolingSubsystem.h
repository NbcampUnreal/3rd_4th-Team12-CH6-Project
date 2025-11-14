// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ObjectPool/PoolManager.h"
#include "ObjectPool/PoolActionDefine.h"
#include "GameData/ObjectPoolDataAsset.h"
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
	T* GetObject(UWorld* World, TFunction<T*(UWorld*)> Constructor, TFunction<void(T*)> OnActivate = nullptr);

	/** 🔹 오브젝트 반환 */
	template<typename T>
	void ReturnObject(T* Object, TFunction<void(T*)> OnDeactivate = nullptr);

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
T* UObjectPoolingSubsystem::GetObject(UWorld* World, TFunction<T*(UWorld*)> Constructor, TFunction<void(T*)> OnActivate)
{
	if constexpr (TIsDerivedFrom<T, AActor>::IsDerived)
		return ActorPool.GetFromPool(World, Constructor, OnActivate);
	else if constexpr (TIsDerivedFrom<T, UAudioComponent>::IsDerived)
		return SoundPool.GetFromPool(World, Constructor, OnActivate);
	else if constexpr (TIsDerivedFrom<T, UNiagaraComponent>::IsDerived)
		return NiagaraPool.GetFromPool(World, Constructor, OnActivate);

	return nullptr;
}

template<typename T>
void UObjectPoolingSubsystem::ReturnObject(T* Object, TFunction<void(T*)> OnDeactivate)
{
	if constexpr (TIsDerivedFrom<T, AActor>::IsDerived)
	{
		ActorPool.ReturnToPool(Object, OnDeactivate);
	}
	else if constexpr (TIsDerivedFrom<T, UAudioComponent>::IsDerived)
	{
		Object->Stop();
		SoundPool.ReturnToPool(Object, OnDeactivate);
	}
	else if constexpr (TIsDerivedFrom<T, UNiagaraComponent>::IsDerived)
	{
		Object->DeactivateImmediate();
		NiagaraPool.ReturnToPool(Object, OnDeactivate);
	}
}

// 🔹 템플릿 함수 명시적 인스턴스화 (UE 빌드에서 필요)
template AActor* UObjectPoolingSubsystem::GetObject<AActor>(UWorld*, TFunction<AActor*(UWorld*)>, TFunction<void(AActor*)>);
template void UObjectPoolingSubsystem::ReturnObject<AActor>(AActor*, TFunction<void(AActor*)>);
template UAudioComponent* UObjectPoolingSubsystem::GetObject<UAudioComponent>(UWorld*, TFunction<UAudioComponent*(UWorld*)>, TFunction<void(UAudioComponent*)>);
template void UObjectPoolingSubsystem::ReturnObject<UAudioComponent>(UAudioComponent*, TFunction<void(UAudioComponent*)>);
template UNiagaraComponent* UObjectPoolingSubsystem::GetObject<UNiagaraComponent>(UWorld*, TFunction<UNiagaraComponent*(UWorld*)>, TFunction<void(UNiagaraComponent*)>);
template void UObjectPoolingSubsystem::ReturnObject<UNiagaraComponent>(UNiagaraComponent*, TFunction<void(UNiagaraComponent*)>);