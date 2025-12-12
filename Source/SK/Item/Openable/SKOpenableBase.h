#pragma once

#include "CoreMinimal.h"
#include "Item/SKInteractableBase.h"
#include "SKOpenableBase.generated.h"

class UCapsuleComponent;

UCLASS()
class SK_API ASKOpenableBase : public ASKInteractableBase
{
	GENERATED_BODY()

public:
	ASKOpenableBase();
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USphereComponent> OverlapCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCapsuleComponent> InteractionPoint;

protected:
	virtual void BeginPlay() override;
};
