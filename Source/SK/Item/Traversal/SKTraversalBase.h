// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/SKDetectableBase.h"
#include "SKTraversalBase.generated.h"

UCLASS()
class SK_API ASKTraversalBase : public ASKDetectableBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASKTraversalBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
