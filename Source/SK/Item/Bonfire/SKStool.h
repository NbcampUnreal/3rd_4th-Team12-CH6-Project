#pragma once

#include "CoreMinimal.h"
#include "SKBonfireBase.h"
#include "SKStool.generated.h"

class ASKBonfire;
class ASKPlayerCharacter;
class UNiagaraComponent;

UCLASS()
class SK_API ASKStool : public ASKBonfireBase
{
	GENERATED_BODY()

public:
	ASKStool();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|StaticMesh")
	TObjectPtr<UStaticMeshComponent> StoolMesh;
	
public:
	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;

};
