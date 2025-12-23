// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AN_Tag_TimedNiagaraEffect.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

void UAN_Tag_TimedNiagaraEffect::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration
)
{
	// 안전 체크
	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	// AbilitySystemComponent 획득 (태그 판별용)
	UAbilitySystemComponent* ASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);

	// 기본적으로 DefaultEffect 사용
	const FTagTimedNiagaraEffect* SelectedEffect = &DefaultEffect;

	// 태그 기반으로 EffectTable 순회
	if (ASC)
	{
		for (const FTagTimedNiagaraEffect& Entry : EffectTable)
		{
			if (ASC->HasMatchingGameplayTag(Entry.RequiredTag))
			{
				SelectedEffect = &Entry;
				break; // 첫 매칭만 사용
			}
		}
	}

	// Niagara가 없으면 실행하지 않음
	if (!SelectedEffect || !SelectedEffect->NiagaraSystem)
	{
		return;
	}

	// Attach 여부에 따라 Spawn 방식 분기
	if (SelectedEffect->bAttached)
	{
		SpawnedNiagara =
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				SelectedEffect->NiagaraSystem,
				MeshComp,
				SelectedEffect->SocketName,
				SelectedEffect->LocationOffset,
				SelectedEffect->RotationOffset,
				EAttachLocation::KeepRelativeOffset,
				SelectedEffect->bFollow
			);
	}
	else
	{
		// 월드 좌표계에 직접 스폰
		const FTransform MeshTransform = MeshComp->GetComponentTransform();
		const FVector WorldLocation =
			MeshTransform.TransformPosition(SelectedEffect->LocationOffset);

		SpawnedNiagara =
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				MeshComp->GetWorld(),
				SelectedEffect->NiagaraSystem,
				WorldLocation,
				SelectedEffect->RotationOffset
			);
	}

	// 공통 후처리
	if (SpawnedNiagara)
	{
		// 스케일 적용
		SpawnedNiagara->SetWorldScale3D(SelectedEffect->Scale);

		// Notify 종료 시 수동으로 끄지만,
		// 예외 상황에서도 자동 제거되도록 AutoDestroy 설정
		SpawnedNiagara->SetAutoDestroy(true);
	}
}

void UAN_Tag_TimedNiagaraEffect::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation
)
{
	// NotifyState 종료 프레임에서 정확히 FX 종료
	if (SpawnedNiagara)
	{
		SpawnedNiagara->Deactivate();
		SpawnedNiagara = nullptr;
	}
}
