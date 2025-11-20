#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SKBreakableBase.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class SK_API ASKBreakableBase : public AActor
{
	GENERATED_BODY()

public:
	ASKBreakableBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UGeometryCollectionComponent> GCComponent;
};
