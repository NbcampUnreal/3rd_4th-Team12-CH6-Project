// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "WeaponDataRow.generated.h" 

USTRUCT(BlueprintType)
struct FSKWeaponDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag WeaponTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> TraceSockets;

	UPROPERTY(EditAnywhere)
	int32 MaxLeftCombo = 1;

	UPROPERTY(EditAnywhere)
	int32 MaxRightCombo = 1;
};