#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interface/SKInteractable.h"
#include "SKInteractableBase.generated.h"

class UBoxComponent;
class USphereComponent;
class UWidgetComponent;

UCLASS()
class SK_API ASKInteractableBase : public AActor, public ISKInteractable
{
	GENERATED_BODY()

public:
	ASKInteractableBase();
	
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USphereComponent> InteractionCollision;

#pragma region Interaction
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Data")
	FSKInteractionData InteractionData;
	
	virtual void GetInteractionData_Implementation(FSKInteractionData& OutData) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
#pragma endregion
	
#pragma region Inventory
protected:
	UFUNCTION(BlueprintCallable, Category = "SK|Inventory")
	void AddToInventory(AActor* Interactor, int32 ItemID, int32 ItemQuantity);

#pragma endregion

#pragma region UI
public:
	UFUNCTION(BlueprintCallable, Category = "SK|UI")
	void OnShowWidget(bool bIsVisible);
		
#pragma endregion 
};
