#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraSystem.h"
#include "SKOpenableData.generated.h"

UCLASS()
class USKOpenableData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Openable")
	TObjectPtr<UNiagaraSystem> OpenNiagara;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Openable")
	TObjectPtr<USoundBase> OpenSound;
};