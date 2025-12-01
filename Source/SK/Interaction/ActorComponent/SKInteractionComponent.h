#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/Interface/SKInteractable.h"
#include "SKInteractionComponent.generated.h"

class ASKInteractableBase;
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

#pragma region Server RPC

	UFUNCTION(Server, Reliable)
	void Server_TryInteract();
	
#pragma endregion

#pragma region Client RPC
	UFUNCTION(Client, Reliable)
	void Client_ToggleInteractableWidget(ASKInteractableBase* Interactable, bool bIsVisible);

#pragma endregion

protected:
	
	void UpdateTargetActor();

	void SetInteractionUI(const bool bIsVisible);

	FTimerHandle UpdateTargetHandle;
	float UpdateInterval = 0.1f;
	
public:
	
	UPROPERTY(BlueprintReadWrite, Replicated)
	FSKInteractionData CurrentInteractionData;

	FORCEINLINE FSKInteractionData& GetInteractionData() { return CurrentInteractionData; }

	FORCEINLINE void SetCurrentTagetActor(ASKInteractableBase* NewActor) { CurrentTargetActor = NewActor; };  
	FORCEINLINE ASKInteractableBase* GetCurrentTagetActor() const { return CurrentTargetActor; };  

	UPROPERTY()
	TSet<ASKInteractableBase*> CandidateActors;

private:
	UPROPERTY(Replicated)
	ASKInteractableBase* CurrentTargetActor;
};
