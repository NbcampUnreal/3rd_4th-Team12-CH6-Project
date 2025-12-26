#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interface/SKInteractable.h"
#include "SKInteractableBase.generated.h"

UENUM()
enum class EObjectType : uint8
{
	Pickup UMETA(DisplayName = "Pickup"),
	Openable UMETA(DisplayName = "Openable"),
	Stool UMETA(DisplayName = "Stool"),
	NPC UMETA(DisplayName = "NPC"),
};

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
protected:
	virtual void GetInteractionData_Implementation(FSKInteractionData& OutData) override;
	
	virtual void Interact_Implementation(AActor* Interactor) override
	{
		// PreExecuteInteraction();
		ExecuteInteraction(Interactor);
	}

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void ExecuteInteraction(AActor* Interactor);
	virtual void ExecuteInteraction_Implementation(AActor* Interactor);

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	// 상호작용 실행 하기 전 호출 필수
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PreExecuteInteraction(AActor* Interactor);

	EObjectType ObjectType;

	UPROPERTY(ReplicatedUsing = OnRep_CanInteract)
	bool bCanInteract;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Data")
	FSKInteractionData InteractionData;

	UFUNCTION()
	void OnRep_CanInteract();

	virtual void HandleCanInteractChanged();
	
#pragma endregion
	
#pragma region Inventory
protected:
	UFUNCTION(BlueprintCallable, Category = "SK|Inventory")
	void AddToInventory(AActor* Interactor, int32 ItemID, int32 ItemQuantity);

#pragma endregion

#pragma region UI
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UWidgetComponent> InteractionWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SK|Data|UI")
	TObjectPtr<UUserWidget> InteractionWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SK|Data|UI")
	FText InteractableText;

	UFUNCTION()
	void ToggleWidget(bool bIsVisible);
	
	UFUNCTION()
	void OnShowWidget(bool bIsVisible);
		
#pragma endregion 
};
