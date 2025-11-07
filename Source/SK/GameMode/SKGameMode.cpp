// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/SKGameMode.h"


#include "Character/SKAllowTeamTag.h"

ASKGameMode::ASKGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorTickEnabled(false);

	DefaultPawnClass = nullptr;
	bUseSeamlessTravel = true;
}

void ASKGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ASKGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UDataTable* ASKGameMode::GetDTAllowedTeamTag()
{
	return DTAllowedTeamTags;
}



FGameplayTagContainer ASKGameMode::GetDTAllowedTeamTagContainer(FGameplayTag ArgGameplayTag)
{
	FGameplayTagContainer ResultContainer;

	if (!DTAllowedTeamTags)
	{
		UE_LOG(LogTemp, Warning, TEXT("AllowTeamDataTable is not set."));
		return ResultContainer;
	}

	TArray<FSKAllowTeamTag*> AllRows;
	DTAllowedTeamTags->GetAllRows<FSKAllowTeamTag>(TEXT("GetAllRows"), AllRows);

	for (FSKAllowTeamTag* Row : AllRows)
	{
		if (Row && Row->AssignedTag == ArgGameplayTag)
		{
			ResultContainer = Row->AllowedTeamTag;
			break;
		}
	}

	if (ResultContainer.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No matching ItemTag found for: %s"), *ArgGameplayTag.ToString());
	}

	return ResultContainer;
}

TSubclassOf<AActor> ASKGameMode::GetDTAllowedTeamTag_Item(FGameplayTag ArgGameplayTag)
{
	TSubclassOf<AActor> ResultItem;

	if (!DTAllowedTeamTags)
	{
		return ResultItem;
	}

	TArray<FSKAllowTeamTag*> AllRows;
	DTAllowedTeamTags->GetAllRows<FSKAllowTeamTag>(TEXT("GetAllRows"), AllRows);

	for (FSKAllowTeamTag* Row : AllRows)
	{
		if (Row && Row->AssignedTag == ArgGameplayTag)
		{
			if (Row && Row->AssignedTag == ArgGameplayTag && Row->ItemClasses.Num() > 0)
			{
				// 랜덤 인덱스 선택
				int32 RandomIndex = FMath::RandRange(0, Row->ItemClasses.Num() - 1);
				ResultItem = Row->ItemClasses[RandomIndex];
				break;
			}
		}
	}

	return ResultItem;
}

float ASKGameMode::GetDTAllowedTeamTag_Period(FGameplayTag ArgGameplayTag)
{
	float result = 0.f;

	if (!DTAllowedTeamTags)
	{
		return result;
	}

	TArray<FSKAllowTeamTag*> AllRows;
	DTAllowedTeamTags->GetAllRows<FSKAllowTeamTag>(TEXT("GetAllRows"), AllRows);

	for (FSKAllowTeamTag* Row : AllRows)
	{
		if (Row && Row->AssignedTag == ArgGameplayTag)
		{
			result = Row->SpawnPeriodSec;
			break;
		}
	}

	return result;
}


