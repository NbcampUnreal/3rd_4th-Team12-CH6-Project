// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "SKMeleeHitBox.generated.h"

UCLASS()
class SK_API ASKMeleeHitBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKMeleeHitBox();

	void InitHitbox(AActor* InOwner, const FGameplayCueParameters& CueParams);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
				   UPrimitiveComponent* OtherComp, int32 BodyIndex,
				   bool bFromSweep, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Collision;

	UPROPERTY(EditAnywhere)
	float HitBoxLifeSpan;


	UPROPERTY(EditAnywhere, Category = "SK|GAS")
	TSubclassOf<UGameplayEffect> DamageGE;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag DamageTag;

	AActor* OwnerActor;

};
