// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundBase.h"
#include "NiagaraSystem.h"
#include "ObjectPoolDataAsset.generated.h"

/** 풀에 들어갈 개별 오브젝트 설정 (에셋 + 개수 쌍) */
USTRUCT(BlueprintType)
struct FActorPoolConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pooling")
	TSoftClassPtr<AActor> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pooling")
	int32 PreloadCount = 0;
};

USTRUCT(BlueprintType)
struct FSoundPoolConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pooling")
	TSoftObjectPtr<USoundBase> SoundAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pooling")
	int32 PreloadCount = 0;
};

USTRUCT(BlueprintType)
struct FNiagaraPoolConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pooling")
	TSoftObjectPtr<UNiagaraSystem> NiagaraAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pooling")
	int32 PreloadCount = 0;
};

/** 풀링 설정 전체를 관리하는 DataAsset */
UCLASS(BlueprintType)
class SK_API UObjectPoolDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pooling")
	TArray<FActorPoolConfig> ActorPools;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pooling")
	TArray<FSoundPoolConfig> SoundPools;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pooling")
	TArray<FNiagaraPoolConfig> NiagaraPools;
	
	// DataAsset 로드 후 자동 SoftReference 해제 및 UObject 로드
	virtual void PostLoad() override;
};
