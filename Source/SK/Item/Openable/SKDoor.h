#pragma once

#include "CoreMinimal.h"
#include "SKOpenableBase.h"
#include "SKDoor.generated.h"

UCLASS()
class SK_API ASKDoor : public ASKOpenableBase
{
	GENERATED_BODY()

public:
	ASKDoor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
};
