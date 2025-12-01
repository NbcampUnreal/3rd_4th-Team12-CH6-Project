#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/Interface/SKInteractable.h"
#include "SKInteractionComponent.generated.h"

class ASKPlayerCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SK_API USKInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USKInteractionComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(Server, Reliable)
	void Server_TryInteract();

	UFUNCTION(Client, Reliable)

	UPROPERTY(BlueprintReadWrite, Replicated)
	FSKInteractionData CurrentInteractionData;

	FORCEINLINE FSKInteractionData& GetInteractionData() { return CurrentInteractionData; }

	FORCEINLINE void SetCurrentTagetActor(AActor* NewActor) { CurrentTargetActor = NewActor; };  

private:
	UPROPERTY(Replicated)
	AActor* CurrentTargetActor;
};
