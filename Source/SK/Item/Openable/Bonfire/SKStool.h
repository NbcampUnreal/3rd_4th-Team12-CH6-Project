#pragma once

#include "CoreMinimal.h"
#include "Item/Openable/SKOpenableBase.h"
#include "SKStool.generated.h"

class ASKBonfire;
class ASKPlayerCharacter;
class UNiagaraComponent;

UCLASS()
class SK_API ASKStool : public ASKOpenableBase
{
	GENERATED_BODY()

public:
	ASKStool();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|StaticMesh")
	TObjectPtr<UStaticMeshComponent> StoolMesh;

#pragma region Interaction
	
public:
	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;

	UPROPERTY()
	ASKBonfire* OwnerBonfire;

protected:
	virtual void OverLapBeginHandle() override;

	virtual void OverLapEndHandle() override;
	
#pragma endregion
};
