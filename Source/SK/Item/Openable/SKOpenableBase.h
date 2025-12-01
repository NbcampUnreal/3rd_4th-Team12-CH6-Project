#pragma once

#include "CoreMinimal.h"
#include "Item/SKInteractableBase.h"
#include "SKOpenableBase.generated.h"

UCLASS()
class SK_API ASKOpenableBase : public ASKInteractableBase
{
	GENERATED_BODY()

public:
	ASKOpenableBase();
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USphereComponent> OverlapCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USceneComponent> InteractionPoint;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
