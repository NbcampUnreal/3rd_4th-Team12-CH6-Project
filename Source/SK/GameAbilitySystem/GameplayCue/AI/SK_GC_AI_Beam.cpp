#include "GameAbilitySystem/GameplayCue/AI/SK_GC_AI_Beam.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"

ASK_GC_AI_Beam::ASK_GC_AI_Beam()
{
	
}

bool ASK_GC_AI_Beam::OnActive_Implementation(AActor* TargetActor, const FGameplayCueParameters& Parameters)
{
	bool bSuper = Super::OnActive_Implementation(TargetActor, Parameters);

	if (IsValid(NiagaraComponent))
	{
		return true;
	}
	
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	if (!IsValid(TargetCharacter))
	{
		return false;
	}

	NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		NiagaraSystem,
		TargetCharacter->GetMesh(),
		TEXT("head_socket"),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTargetIncludingScale,
		false
	);
	
	return bSuper;
}

bool ASK_GC_AI_Beam::WhileActive_Implementation(AActor* TargetActor, const FGameplayCueParameters& Parameters)
{
	bool bSuper = Super::WhileActive_Implementation(TargetActor, Parameters);
	
	return bSuper;
}

bool ASK_GC_AI_Beam::OnRemove_Implementation(AActor* TargetActor, const FGameplayCueParameters& Parameters)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Remove Effect");
	if (IsValid(NiagaraComponent))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "NC Valid");
		NiagaraComponent->Deactivate();
		NiagaraComponent->DestroyComponent();
	}
	
	return Super::OnRemove_Implementation(TargetActor, Parameters);
}
