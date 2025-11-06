// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"

/**
 * 범용 오브젝트 풀 템플릿
 */
template <typename T>
class SK_API PoolManager
{
public:
    // 생성자 델리게이트: 각 타입에 맞게 객체를 생성할 때 사용
    using FCreateFunc = TFunction<T* (UWorld* World)>;

    // 풀 초기화
    void InitializePool(UWorld* World, int32 Count, FCreateFunc Constructor)
    {
        if (!World || !Constructor) return;

        for (int32 i = 0; i < Count; ++i)
        {
            T* NewObj = Constructor(World);

            if constexpr (TIsDerivedFrom<T, UAudioComponent>::IsDerived)
            {
                NewObj->OnAudioFinishedNative.AddLambda([this](UAudioComponent* FinishedComponent)
                {
                    this->ReturnToPool(static_cast<T*>(FinishedComponent));
                });
            }
            
            if (IsValid(NewObj))
            {
                PrepareForInactive(NewObj);
                InactivePool.Add(NewObj);
            }
        }

        UE_LOG(LogTemp, Log, TEXT("[TPoolManager<%s>] Initialized with %d objects"), *T::StaticClass()->GetName(), Count);
    }

    // 풀에서 객체 가져오기 (없으면 새롭게 생성)
    T* GetFromPool(UWorld* World, FCreateFunc Constructor)
    {
        if (!World) return nullptr;

        T* Obj = nullptr;

        if (InactivePool.Num() > 0)
        {
            Obj = InactivePool.Pop();
            ActivePool.Add(Obj);
            PrepareForActive(Obj);
            UE_LOG(LogTemp, Log, TEXT("[TPoolManager] GetFromPool : 기존 오브젝트 재사용"));
        }
        else if (Constructor)//없으면 생성
        {
            Obj = Constructor(World);
            if (IsValid(Obj))
            {
                if constexpr (TIsDerivedFrom<T, UAudioComponent>::IsDerived)
                {
                    Obj->OnAudioFinishedNative.AddLambda([this](UAudioComponent* FinishedComponent)
                    {
                        this->ReturnToPool(static_cast<T*>(FinishedComponent));
                    });
                }
                
                ActivePool.Add(Obj);
                PrepareForActive(Obj);
                UE_LOG(LogTemp, Log, TEXT("[TPoolManager] GetFromPool :새 오브젝트 생성"));
            }
        }

        return Obj;
    }

    // 객체 반환
    void ReturnToPool(T* Obj)
    {
        if (!IsValid(Obj)) return;

        //활성화된 풀 리스트에 있는지 체크 
        const bool bWasActive = ActivePool.Contains(Obj);
        
        //풀에 없는 오브젝트를 반환하려고하면 무시
        if (!bWasActive)
        {
            UE_LOG(LogTemp, Log, TEXT("[TPoolManager] ReturnToPool : Tried to return an object not managed by the pool: %s"), *Obj->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("[TPoolManager] ReturnToPool : %s"), *Obj->GetName());
        }

        ActivePool.Remove(Obj);
        PrepareForInactive(Obj);
        InactivePool.Add(Obj);
    }

    // 전체 풀 정리 (월드 언로드 시)
    void ClearAllPools()
    {
        for (T* Obj : ActivePool)
            SafeDestroy(Obj);

        for (T* Obj : InactivePool)
            SafeDestroy(Obj);

        ActivePool.Empty();
        InactivePool.Empty();
    }

private:
    // 안전한 파괴
    void SafeDestroy(UObject* Obj)
    {
        if (!Obj) return;
        if constexpr (TIsDerivedFrom<T, AActor>::IsDerived)
        {
            if (AActor* Actor = Cast<AActor>(Obj))
                Actor->Destroy();
        }
        else
        {
            Obj->ConditionalBeginDestroy();
        }
    }

    // 비활성 상태 준비
    void PrepareForInactive(T* Obj)
    {
        if constexpr (TIsDerivedFrom<T, AActor>::IsDerived)
        {
            UE_LOG(LogTemp, Log, TEXT("[TPoolManager] PrepareForInactive 비활성화"));
            Obj->SetActorHiddenInGame(true);
            Obj->SetActorEnableCollision(false);
            Obj->SetActorTickEnabled(false);
        }
        else if constexpr (TIsDerivedFrom<T, UAudioComponent>::IsDerived)
        {
            Obj->Stop();
            Obj->SetActive(false);
        }
        else if constexpr (TIsDerivedFrom<T, UNiagaraComponent>::IsDerived)
        {
            Obj->DeactivateImmediate();
            Obj->ResetSystem();
            Obj->SetAutoActivate(false);
            Obj->SetVisibility(false);
            Obj->SetActive(false);
        }
    }

    // 활성 상태 준비
    void PrepareForActive(T* Obj)
    {
        if constexpr (TIsDerivedFrom<T, AActor>::IsDerived)
        {
            UE_LOG(LogTemp, Log, TEXT("[TPoolManager] PrepareForActive 활성화"));
            Obj->SetActorHiddenInGame(false);
            Obj->SetActorEnableCollision(true);
            Obj->SetActorTickEnabled(true);
        }
        else if constexpr (TIsDerivedFrom<T, UAudioComponent>::IsDerived)
        {
            Obj->SetActive(true);
        }
        else if constexpr (TIsDerivedFrom<T, UNiagaraComponent>::IsDerived)
        {
            Obj->ResetSystem();
            Obj->SetVisibility(true);
            Obj->Activate(true);
        }
    }
    
    //비활성화 목록
    TArray<T*> InactivePool;

    //활성화 목록
    TArray<T*> ActivePool;
};
