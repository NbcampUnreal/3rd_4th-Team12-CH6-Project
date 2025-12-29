#pragma once

#include "CoreMinimal.h"
#include "SKOpenableBase.h"
#include "SKDoubledoor.generated.h"

UCLASS()
class SK_API ASKDoubledoor : public ASKOpenableBase
{
	GENERATED_BODY()

public:
	ASKDoubledoor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMeshLeft;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMeshRight;
};
