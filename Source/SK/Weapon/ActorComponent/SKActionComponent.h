#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Controller/SKPlayerController.h"
#include "SKActionComponent.generated.h"

class USKWeaponAnimData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SK_API USKActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USKActionComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponAnimData(USKWeaponAnimData* NewWeaponActionData);
	
	FORCEINLINE USKWeaponAnimData* GetWeaponActionData() const { return CurrentWeaponActionData; }
	
protected:
	UPROPERTY()
	USKWeaponAnimData* CurrentWeaponActionData;

#pragma region MovementInfo

public:
	UFUNCTION(Server, Reliable)
	void Server_SetMovementInfo(const FVector2D NewInputVector, const EMoveDirection NewMovementDirection);

	UPROPERTY(Replicated)
	FVector2D CurrentInputVector;

	UPROPERTY(Replicated)
	EMoveDirection CurrentMovementDirection;

#pragma endregion

	
#pragma region Dodge

public:
	FRotator GetDodgeRotator() const;

	void TryDodge();

protected:
	UFUNCTION(Server, Reliable)
	void Server_ExecuteDodge(FName DodgeType);
	
	bool CheckDoubleTap();

	void CloseGate();
	
private:
	FTimerHandle GateTimerHandle;
	bool bIsGateOpen;
	
#pragma endregion
};
