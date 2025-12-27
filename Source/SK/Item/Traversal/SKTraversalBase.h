#pragma once

#include "CoreMinimal.h"
#include "Item/SKDetectableBase.h"
#include "SKTraversalBase.generated.h"

enum class EForceMoveMode : uint8;

UCLASS()
class SK_API ASKTraversalBase : public ASKDetectableBase
{
	GENERATED_BODY()

public:
	ASKTraversalBase();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|AvoidObstacle")
	EForceMoveMode ForceMoveMode;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ObstacleMesh;
};
