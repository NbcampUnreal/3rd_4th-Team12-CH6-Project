#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "SK_GC_AI_Beam.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class SK_API ASK_GC_AI_Beam : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;


public:
	ASK_GC_AI_Beam();
	
protected:
	virtual bool OnActive_Implementation(AActor* TargetActor, const FGameplayCueParameters& Parameters) override;

	virtual bool WhileActive_Implementation(AActor* TargetActor, const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(AActor* TargetActor, const FGameplayCueParameters& Parameters) override;
};
