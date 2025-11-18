#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "UObject/Interface.h"
#include "SKInteractable.generated.h"

USTRUCT(BlueprintType)
struct FSKInteractionData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InteractionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> GrantedAbility;
	
};

UINTERFACE(MinimalAPI)
class USKInteractable : public UInterface
{
	GENERATED_BODY()
};

class SK_API ISKInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void GetInteractionData(FSKInteractionData& OutData);
};
