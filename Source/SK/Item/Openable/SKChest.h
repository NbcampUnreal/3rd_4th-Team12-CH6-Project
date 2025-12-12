#pragma once

#include "CoreMinimal.h"
#include "SKOpenableBase.h"
#include "Item/Pickup/SKPickupItem.h"
#include "SKChest.generated.h"

UCLASS()
class SK_API ASKChest : public ASKOpenableBase
{
	GENERATED_BODY()

public:
	ASKChest();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ChestMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LidMesh;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|ItemInfo")
	TArray<FSKItemInfo> Items;
};
