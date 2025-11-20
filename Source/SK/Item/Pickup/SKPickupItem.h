#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Item/SKInteractableBase.h"
#include "SKPickupItem.generated.h"

class UWidgetComponent;
class USphereComponent;
class USKPickupItemData;

UCLASS()
class SK_API ASKPickupItem : public ASKInteractableBase
{
	GENERATED_BODY()

public:
	ASKPickupItem();

protected:
	virtual void BeginPlay() override;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	// TObjectPtr<UParticleSystemComponent> Particle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UNiagaraComponent> ItemNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Data")
	TObjectPtr<USKPickupItemData> PickupData;

	void PlayPickupSound();
public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact_Implementation(AActor* Interactor) override;
};