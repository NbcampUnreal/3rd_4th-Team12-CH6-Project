#pragma once

#include "CoreMinimal.h"
#include "Animation/SKBaseAnimInstance.h"
#include "Components/ActorComponent.h"
#include "Interaction/Interface/SKInteractable.h"
#include "SKInteractionComponent.generated.h"

class UWidgetComponent;
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

	UFUNCTION(Server, Reliable)
	void Server_ActivateInteractionAbility(TSubclassOf<UGameplayAbility> Ability) ;
	
#pragma endregion

#pragma region Client RPC
	UFUNCTION(Client, Reliable)
	void Client_ToggleInteractableWidget(UWidgetComponent* Widget, bool bIsVisible);

#pragma endregion

#pragma region Interaction
	
protected:
	
	void UpdateTargetActor();

	void SetInteractionUI(const bool bIsVisible);

	void ActivateInteractionAbility(TSubclassOf<UGameplayAbility> Ability) const;

	FTimerHandle UpdateTargetHandle;
	float UpdateInterval = 0.1f;
	
public:
	UFUNCTION(Server, Reliable)
	void Server_SetForceMoveMode(const EForceMoveMode NewForceMoveMode);
	
	UFUNCTION(Server, Reliable)
	void Server_SetForceMove(const bool NewForceMode);

	UFUNCTION(BlueprintCallable)
	EForceMoveMode GetForceMoveMode() const { return ForceMoveMode; };
	
	UFUNCTION(BlueprintCallable)
	bool GetForceMove() const { return bForceMove; };

	FORCEINLINE void SetInteractionData(const FSKInteractionData& NewInteractionData) { CurrentInteractionData = NewInteractionData; }
	FORCEINLINE const FSKInteractionData& GetInteractionData() { return CurrentInteractionData; }

	FORCEINLINE void SetCurrentTagetActor(ASKInteractableBase* NewActor) { CurrentTargetActor = NewActor; };  
	FORCEINLINE ASKInteractableBase* GetCurrentTargetActor() const { return CurrentTargetActor; };  

	UFUNCTION()
	void OnRep_CurrentInteractionData();
	
	UPROPERTY()
	TSet<ASKInteractableBase*> CandidateActors;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentInteractionData)
	FSKInteractionData CurrentInteractionData;
	
	UPROPERTY(Replicated)
	ASKInteractableBase* CurrentTargetActor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	EForceMoveMode ForceMoveMode;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	uint8 bForceMove : 1;
	
#pragma endregion
};
