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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USceneComponent> InteractionPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UBoxComponent> TraceCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Data")
	FSKInteractionData InteractionData;
	
	virtual void GetInteractionData_Implementation(FSKInteractionData& OutData) override;

	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
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
