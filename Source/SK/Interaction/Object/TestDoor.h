#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interface/IInteractable.h"
#include "TestDoor.generated.h"

UCLASS()
class SK_API ATestDoor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ATestDoor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, Category = "Components")
	class UBoxComponent* InteractionBox;

	UPROPERTY(VisibleAnywhere, Category = "Door")
	bool bIsOpen;

	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenAngle;

	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenSpeed;

	FRotator TargetRotation;
	FRotator InitialRotation;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual void GetInteractionData_Implementation(FInteractionData& OutData) override;
	
	UPROPERTY(EditAnywhere, Category = "Door")
	FInteractionData InteractionData;
};