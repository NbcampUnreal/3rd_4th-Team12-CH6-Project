// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/ObjectPoolDataAsset.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UObjectPoolDataAsset::PostLoad()
{
	Super::PostLoad();

	UE_LOG(LogTemp, Log, TEXT("[UObjectPoolDataAsset] PostLoad called: %s"), *GetName());

	// -------------------------
	// 액터 풀
	// -------------------------
	for (int32 i = 0; i < ActorPools.Num(); ++i)
	{
		auto& Pool = ActorPools[i];

		if (!Pool.ActorClass.IsValid() && Pool.ActorClass.IsNull() == false)
		{
			UClass* LoadedClass = Pool.ActorClass.LoadSynchronous();
			UE_LOG(LogTemp, Log, TEXT("[UObjectPoolDataAsset] ActorClass[%d] loaded: %s"),
				i, LoadedClass ? *LoadedClass->GetName() : TEXT("FAILED"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[UObjectPoolDataAsset] ActorClass[%d] already valid or null"), i);
		}
	}

	// -------------------------
	// 사운드 풀
	// -------------------------
	for (int32 i = 0; i < SoundPools.Num(); ++i)
	{
		auto& Pool = SoundPools[i];

		if (!Pool.SoundAsset.IsValid() && Pool.SoundAsset.IsNull() == false)
		{
			USoundBase* Sound = Pool.SoundAsset.LoadSynchronous();
			UE_LOG(LogTemp, Log, TEXT("[UObjectPoolDataAsset] SoundAsset[%d] loaded: %s"),
				i, Sound ? *Sound->GetName() : TEXT("FAILED"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[UObjectPoolDataAsset] SoundAsset[%d] already valid or null"), i);
		}
	}

	// -------------------------
	// 나이아가라 풀
	// -------------------------
	for (int32 i = 0; i < NiagaraPools.Num(); ++i)
	{
		auto& Pool = NiagaraPools[i];

		if (!Pool.NiagaraAsset.IsValid() && Pool.NiagaraAsset.IsNull() == false)
		{
			UNiagaraSystem* Niagara = Pool.NiagaraAsset.LoadSynchronous();
			UE_LOG(LogTemp, Log, TEXT("[UObjectPoolDataAsset] NiagaraAsset[%d] loaded: %s"),
				i, Niagara ? *Niagara->GetName() : TEXT("FAILED"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[UObjectPoolDataAsset] NiagaraAsset[%d] already valid or null"), i);
		}
	}
}