#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "GameMode/MatchState/DungeonMatchState.h"
#include "Perception/AIPerceptionTypes.h"
#include "GenericTeamAgentInterface.h"
#include "SKAIController.generated.h"

class UStateTreeAIComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Damage;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Perception")
	TObjectPtr<UAISenseConfig_Damage> DamageConfig;
	
	UPROPERTY()
	UAbilitySystemComponent* OwningASC;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> TargetActors;
	
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;

	FGenericTeamId CachedTeamID;

public:
	FTimerHandle FindClosestTargetTimerHandle;

	
public:
	ASKAIController();

	TObjectPtr<AActor> GetTargetActor() const;

	bool CheckClose(float AdditionalCapsuleRadiusSum);

	FVector GetPredictedTargetLocation(float PredictionTime) const;

	FVector GetPredictedToTargetDirection(const FVector& PredictedLocation) const;
	
	void AddTag(FGameplayTag Tag) const;

	void RemoveTag(FGameplayTag Tag) const;
	
	void SendEventToASC(AActor* LocalInstigator, AActor* LocalTargetActor, FGameplayTag EventTag) const;

	void FindClosestTarget(); 

	virtual FGenericTeamId GetGenericTeamId() const override { return CachedTeamID; }

	// 팀 시스템 - Perception 이 팀 적대 관계를 알기 위해 반드시 필요
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	uint8 GetTeamIDFromActor(const AActor& Other) const;
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void BeginPlay() override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void OnDungeonStateChanged(EDungeonMatchState NewState);

	uint8 ConvertTeamTagToID(const FGameplayTagContainer& InTags) const;
};
