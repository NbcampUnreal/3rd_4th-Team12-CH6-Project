#pragma once

#include "CoreMinimal.h"
#include "Item/SKInteractableBase.h"
#include "SKDoor.generated.h"

UCLASS()
class SK_API ASKDoor : public ASKInteractableBase
{
	GENERATED_BODY()

public:
	ASKDoor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
};
