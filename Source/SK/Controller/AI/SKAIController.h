#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SKAIController.generated.h"

class UStateTreeAIComponent;
class UStateTree;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class SK_API ASKAIController : public AAIController
{
	GENERATED_BODY()

protected:
	// StateTree
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|StateTree")
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UStateTree> StateTreeAsset;

	// Perception
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI|Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

private:
	AActor* TargetActor;
	
public:
	ASKAIController();
	
	UFUNCTION(BlueprintCallable)
	AActor* GetTargetActor() const;
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnDungeonStarted();
};
