#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "SK_GC_AI_BaseStatic.generated.h"

class UNiagaraSystem;

UCLASS()
class SK_API USK_GC_AI_BaseStatic : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EffectScale")
	float EffectScale = 1.f;
	
	
public:
	USK_GC_AI_BaseStatic();

protected:
	virtual bool OnExecute_Implementation(AActor* TargetActor, const FGameplayCueParameters& Parameters) const override;
};
