// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/SKDetectableBase.h"
#include "SKBonfire.generated.h"

class UNiagaraSystem;
class UNavModifierComponent;
class UNiagaraComponent;
class ASKStool;

UCLASS()
class SK_API ASKBonfire : public ASKDetectableBase
{
	GENERATED_BODY()

public:
	ASKBonfire();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|StaticMesh")
	TObjectPtr<UStaticMeshComponent> BonfireMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|Niagara")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|NavModifier")
	TObjectPtr<UNavModifierComponent> NavModifierComponent;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bonfire")
	bool bIsDefaultBonfire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bonfire")
	TSubclassOf<UGameplayEffect> ResetGameplayEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bonfire")
	TObjectPtr<UNiagaraSystem> SpawnNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bonfire")
	TObjectPtr<UNiagaraSystem> RestNiagara;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnEffect(FTransform Transform);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RestEffect(FTransform Transform);
	
	UPROPERTY()
	TSet<UWidgetComponent*> DetectWidgets;
	
	void ResetBonfire(ASKPlayerCharacter* PlayerCharacter);
};
