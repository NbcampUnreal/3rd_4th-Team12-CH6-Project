// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameplayTagContainer.h"
#include "SKGameMode.generated.h"


class ASKCharacter;
/**
 * 
 */
UCLASS()
class SK_API ASKGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	ASKGameMode();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	

#pragma region SPAWN


public:
	UDataTable* GetDTAllowedTeamTag();
	FGameplayTagContainer GetDTAllowedTeamTagContainer(FGameplayTag ArgGameplayTag);
	TSubclassOf<AActor> GetDTAllowedTeamTag_Item(FGameplayTag ArgGameplayTag);
	float GetDTAllowedTeamTag_Period(FGameplayTag ArgGameplayTag);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|DataTable")
	UDataTable* AllowedTeamTags;
#pragma endregion
};
