#pragma once

#include "CoreMinimal.h"
#include "Item/SKInteractableBase.h"
#include "SKOpenableBase.generated.h"

class ASKPlayerCharacter;
class UCapsuleComponent;

UCLASS()
class SK_API ASKOpenableBase : public ASKInteractableBase
{
	GENERATED_BODY()

public:
	ASKOpenableBase();
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USphereComponent> DetectCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCapsuleComponent> InteractionPoint;

protected:
	virtual void BeginPlay() override;

	virtual void OverLapBeginHandle();

	virtual void OverLapEndHandle();

#pragma region Near UI
	UFUNCTION()
	void OnDetectBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnDetectEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void HandleCanInteractChanged() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UWidgetComponent> DetectWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SK|Data|UI")
	TObjectPtr<UUserWidget> DetectWidgetClass;
	
#pragma endregion
};
