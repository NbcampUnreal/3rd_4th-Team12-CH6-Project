#pragma once

#include "CoreMinimal.h"
#include "Item/SKInteractableBase.h"
#include "SKChest.generated.h"

UCLASS()
class SK_API ASKChest : public ASKInteractableBase
{
	GENERATED_BODY()

public:
	ASKChest();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ChestMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LidMesh;
};
