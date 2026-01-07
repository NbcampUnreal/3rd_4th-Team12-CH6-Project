#include "GameAbilitySystem/GameplayCue/AI/SK_GC_AI_BaseStatic.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

USK_GC_AI_BaseStatic::USK_GC_AI_BaseStatic()
{
	
}

bool USK_GC_AI_BaseStatic::OnExecute_Implementation(AActor* TargetActor, const FGameplayCueParameters& Parameters) const
{
	bool bSuper =  Super::OnExecute_Implementation(TargetActor, Parameters);

	if (!NiagaraSystem)
	{
		return false;
	}

	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			TargetActor->GetWorld(),
			NiagaraSystem, 
			Parameters.Location
			);
	if (!IsValid(NiagaraComponent))
	{
		return false;
	}

	NiagaraComponent->SetRelativeScale3D(FVector(EffectScale));

	return bSuper;
}
