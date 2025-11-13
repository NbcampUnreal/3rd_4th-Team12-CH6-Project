#pragma once
#include "Interaction/Interface/SKInteractable.h"
#include "CoreMinimal.h"
#include "SKPickupItem.generated.h"

class USphereComponent;
class USKPickupItemData;

UCLASS()
class SK_API ASKPickupItem : public AActor, public ISKInteractable
{
	GENERATED_BODY()

public:
	ASKPickupItem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	// TObjectPtr<UParticleSystemComponent> Particle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Staticmesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USphereComponent> TraceCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Data")
	TObjectPtr<USKPickupItemData> PickupData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Data")
	FSKInteractionData InteractionData;

	void PlayPickupSound();
	
public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact_Implementation(AActor* Interactor) override;
	
	virtual void GetInteractionData_Implementation(FSKInteractionData& OutData) override;
};