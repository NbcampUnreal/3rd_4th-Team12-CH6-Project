#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"

/**
 * 풀 오브젝트의 기본 행동 가이드라인 (활성화 / 비활성화)
 * 사용자는 아래 정해진 프리셋을 그대로 쓰거나, 커스텀 람다를 조합 가능.
 */
namespace PoolActionDefine
{
	namespace Actor
	{
		static auto Activate = [](AActor* A)
		{
			if (!A) return;
			A->SetActorHiddenInGame(false);
			A->SetActorEnableCollision(true);
			A->SetActorTickEnabled(true);
		};

		static auto Deactivate = [](AActor* A)
		{
			if (!A) return;
			A->SetActorHiddenInGame(true);
			A->SetActorEnableCollision(false);
			A->SetActorTickEnabled(false);
		};
	}

	namespace Audio
	{
		static auto Activate = [](UAudioComponent* Comp)
		{
			if (!Comp) return;
			Comp->SetActive(true);
		};

		static auto Deactivate = [](UAudioComponent* Comp)
		{
			if (!Comp) return;
			Comp->Stop();
			Comp->SetActive(false);
		};
	}

	namespace Niagara
	{
		static auto Activate = [](UNiagaraComponent* Comp)
		{
			if (!Comp) return;
			Comp->ResetSystem();
			Comp->SetVisibility(true);
			Comp->Activate(true);
		};

		static auto Deactivate = [](UNiagaraComponent* Comp)
		{
			if (!Comp) return;
			Comp->DeactivateImmediate();
			Comp->ResetSystem();
			Comp->SetAutoActivate(false);
			Comp->SetVisibility(false);
			Comp->SetActive(false);
		};
	}
}