#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "GameMode/MatchState/DungeonMatchState.h"
#include "Perception/AIPerceptionTypes.h"
#include "SKAIController.generated.h"

class UStateTreeAIComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAbilitySystemComponent;

UCLASS()
class SK_API ASKAIController : public AAIController
{
	GENERATED_BODY()

protected:
	// StateTree
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="StateTree")
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent;

	// Perception
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY()
	UAbilitySystemComponent* OwningASC;
	
	UPROPERTY()
	AActor* TargetActor;

public:
	ASKAIController();
	
	AActor* GetTargetActor() const;

	void AddTag(FGameplayTag Tag) const;

	void RemoveTag(FGameplayTag Tag) const;
	
	void SendEventToASC(AActor* LocalInstigator, AActor* LocalTargetActor, FGameplayTag EventTag) const;

	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void BeginPlay() override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void OnDungeonStateChanged(EDungeonMatchState NewState);
};
