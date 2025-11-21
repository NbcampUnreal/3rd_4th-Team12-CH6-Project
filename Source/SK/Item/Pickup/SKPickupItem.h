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

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void InitializePickup(USKPickupItemData* InPickupData, int32 Count);

protected:
	virtual void BeginPlay() override;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	// TObjectPtr<UParticleSystemComponent> Particle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UNiagaraComponent> ItemNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Data", ReplicatedUsing=OnRep_PickupData)
	TObjectPtr<USKPickupItemData> PickupData;

	/** 드랍된 아이템 개수 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
	int32 ItemCount = 1;

	USoundBase* GetPickupSound() const;

	int32 GetItemID() const;

	UFUNCTION()
	void OnRep_PickupData();
	
public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact_Implementation(AActor* Interactor) override;
};