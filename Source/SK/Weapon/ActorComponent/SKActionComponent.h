#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SKActionComponent.generated.h"


class USKWeaponActionData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SK_API USKActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USKActionComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponActionData(USKWeaponActionData* NewWeaponActionData);
	
	FORCEINLINE USKWeaponActionData* GetWeaponActionData() const { return CurrentWeaponActionData; }
	
protected:
	UPROPERTY()
	USKWeaponActionData* CurrentWeaponActionData;
	
#pragma region Dodge

public:
	FRotator GetDodgeRotator() const;

	UFUNCTION(Server, Reliable)
	void Server_ExecuteDodge();

protected:
	
	UFUNCTION()
	bool CheckDoubleTab();
	
#pragma endregion
};
