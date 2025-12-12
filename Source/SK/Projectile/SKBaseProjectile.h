#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SKBaseProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UGameplayEffect;

UCLASS()
class SK_API ASKBaseProjectile : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float InitialSpeed = 3000.0f;

	
public:	
	ASKBaseProjectile();

	void LaunchProjectile(const FVector& LaunchDirection);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
};
