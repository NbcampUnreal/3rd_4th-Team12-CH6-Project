// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "SK.h"
#include "GameFramework/Actor.h"
#include "SKBaseObject.generated.h"

class UAbilitySystemComponent;

class USphereComponent;
class UStaticMeshComponent;
class UGameplayEffect;

UCLASS()
class SK_API ASKBaseObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKBaseObject();

	virtual void Tick(float DeltaTime) override;
	
	const FGameplayTagContainer& GetAllowedTeamTag() const { return AllowedTeamTag; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);


	void DefaultConstructor();
	bool HasAllowedTeamTag(UAbilitySystemComponent* ASC) const;
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "SK|Components")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "SK|Components")
	UStaticMeshComponent* MeshComp;

	// 적용할 GameplayEffect (블루프린트에서 할당)
	UPROPERTY(EditAnywhere, Category = "SK|GAS")
	TSubclassOf<UGameplayEffect> CollisionEffect;

	UPROPERTY(EditAnywhere, Category="SK|GAS")
	FGameplayTag ItemTag;

	//충돌이펙트는 있지만 부서지지않게 하고싶을경우 false
	UPROPERTY(EditAnywhere, Category="SK|GAS")
	bool bDestoryFlag  = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|GAS")
	FGameplayTagContainer AllowedTeamTag;

	//시각적 투사체
	UE_GET_SET_INIT(bool,bIsCosmetic,false,EditAnywhere,BlueprintReadWrite)

private:
	void SetAllowedTeamTag();
	void ApplyEffectAndDestroy(UAbilitySystemComponent* ASC);
private:
	FGameplayTag OwnerTeamTag;
	FTransform StartTransform = FTransform();
	
};
