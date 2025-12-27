#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Controller/SKPlayerController.h"
#include "SKActionComponent.generated.h"

class ASKInteractableBase;
class UGameplayEffect;
class UEquipmentInstance;
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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region Weapon Data
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetWeaponAnimData(USKWeaponAnimData* NewWeaponAnimData);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetWeaponData(USKWeaponData* NewWeaponData);
	
	FORCEINLINE USKWeaponAnimData* GetWeaponAnimData() const { return CurrentWeaponAnimData; }
	
protected:
	UPROPERTY(Replicated)
	USKWeaponAnimData* CurrentWeaponAnimData;
#pragma endregion

#pragma region MovementInfo

public:
	UFUNCTION(Server, Reliable)
	void Server_SetMovementInfo(const FVector2D NewInputVector, const EMoveDirection NewMoveDirection);

	UPROPERTY(Replicated)
	FVector2D CurrentInputVector;

	UPROPERTY(ReplicatedUsing = OnRep_OnMoveDirectionChange)
	EMoveDirection CurrentMoveDirection;

	UFUNCTION()
	void OnRep_OnMoveDirectionChange();

protected:

	void SetMoveDirection();

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

#pragma region Unequip
public:	
	void OnOwnerPossessed();

	void AttachWeapon(const TArray<FName> SocketNames);

	void OnCombatStart();

	UPROPERTY(Replicated)
	TArray<AActor*> WeaponActors;
	
	UPROPERTY()
	float LastCombatTime;
	
protected:
	void CheckAutoUnEquipped();
	
	UPROPERTY()
	float AutoUnequipDelay = 10.0f;

public:
	UPROPERTY()
	FTimerHandle AutoUnEquippedTimerHandle;
	
#pragma endregion

#pragma region Bonfire
	
public:
	UFUNCTION(Server, Reliable)
	void Server_SetIgnoreCollision(bool bIgnore);

	void SetBonfireWidgetVisibility(ASKInteractableBase* TargetActor, bool bOnWidget);

	UPROPERTY()
	ASKInteractableBase* InteractedStool;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_IgnoreCollision)
	bool bIgnoreCollision;
	
	UFUNCTION()
	void OnRep_IgnoreCollision();

	void ApplyCollisionSetting();
#pragma endregion
};
