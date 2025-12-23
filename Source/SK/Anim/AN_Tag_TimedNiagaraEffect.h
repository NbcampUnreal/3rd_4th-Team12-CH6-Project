// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AN_Tag_TimedNiagaraEffect.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

USTRUCT(BlueprintType)
struct FTagTimedNiagaraEffect
{
	GENERATED_BODY()

	/** 이 태그가 있으면 이 이펙트 세트를 사용 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tag")
	FGameplayTag RequiredTag;

	/** 실행할 Niagara System */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Niagara")
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

	/** 부착할 소켓 이름 (비어 있으면 Mesh Root) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Transform")
	FName SocketName;

	/** 위치 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Transform")
	FVector LocationOffset = FVector::ZeroVector;

	/** 회전 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Transform")
	FRotator RotationOffset = FRotator::ZeroRotator;

	/** 스케일 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Transform")
	FVector Scale = FVector(1.f);

	/** Mesh에 Attach 할지 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attach")
	bool bAttached = true;

	/** Attach 상태에서 Mesh 움직임을 따라갈지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attach")
	bool bFollow = true;
};

/**
 * 태그 기반 Timed Niagara Effect
 *
 * 몽타주 타임라인에서 시작/종료 프레임 직접 제어
 * GameplayTag에 따라 서로 다른 Niagara 실행
 */
UCLASS(meta=(DisplayName="Tag Timed Niagara Effect"))
class SK_API UAN_Tag_TimedNiagaraEffect : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	/** NotifyState 시작 시 호출 (몽타주에서 지정한 시작 프레임) */
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration
	) override;
	
	/** NotifyState 종료 시 호출 (몽타주에서 지정한 종료 프레임) */
	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation
	) override;

protected:
	/**
	 * 태그 → Timed Niagara Effect 세트
	 * - 위에서부터 순서대로 검사
	 * - 처음 매칭되는 항목 하나만 사용
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	TArray<FTagTimedNiagaraEffect> EffectTable;

	/**
	 * 어떤 태그도 매칭되지 않았을 때 사용되는 기본 이펙트
	 * (일반 Guard Block FX 용도)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	FTagTimedNiagaraEffect DefaultEffect;

private:
	/** 현재 NotifyState 동안 재생 중인 Niagara 컴포넌트 */
	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> SpawnedNiagara;
};
