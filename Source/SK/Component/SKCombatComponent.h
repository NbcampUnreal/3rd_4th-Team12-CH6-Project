// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SKCombatComponent.generated.h"


 struct FSKWeaponDataRow;

USTRUCT(BlueprintType)
struct FSKComboState
{
	GENERATED_BODY()

	//  Replicated 영역 (서버 관리)
	UPROPERTY()
	FGameplayTag WeaponTag;

	// UPROPERTY()
	// FGameplayTag AttackTypeTag;

	UPROPERTY()
	int32 ComboIndex = 0;

	//  로컬영역
	UPROPERTY(NotReplicated)
	bool bIsAttacking = false;

	UPROPERTY(NotReplicated)
	bool bCanNextCombo = false;

	UPROPERTY(NotReplicated)
	bool bBufferedAttack = false;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SK_API USKCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USKCombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	bool GetIsAttacking();
	void ResetComboState();
	bool CheckMaxComboIndex(bool bLeft = true);
	FGameplayTag GetLeftATKTag() const;
	int32 GetComboIndex() const;

	UFUNCTION(Server, Reliable)
	void Server_LeftAttackInput();
	UFUNCTION(Server, Reliable)
	void Server_Notify_StopAttackTrace();
	UFUNCTION(Server, Reliable)
	void Server_OnATKEndNotify(bool bLeft);
	UFUNCTION(Client, Reliable)
	void Client_PlayMontage(UAnimMontage* Montage);
	UFUNCTION(Server, Reliable)
	void Server_IncreaseComboIndex(bool bLeft = true);
	UFUNCTION(Server, Reliable)
	void Server_ResetComboIndex(int32 NewIndex);


	void StartTrace();
	void StopTrace();
	void PerformTrace(float DeltaTime);
	
	void ResetCombo(bool bLeft);

	void ClearHitActor();
	void SetIsTraced(bool ArgIsTracing);
	const TArray<AActor*>& GetHitActors();

	void InitializeWeaponData(const FSKWeaponDataRow* Row);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void ActivateLeftAttackGA();
private:
	int32 GetMaxComboIndex(bool bLeft);
	
	UPROPERTY()
	FSKComboState ComboState;

	TArray<FName> TraceSockets;
	TArray<FVector> PrevSocketLocations;

	int32 MaxLeftComboIndex = 1;
	int32 MaxRightComboIndex = 1;

	bool bIsTracing = false;

	UPROPERTY()
	TArray<AActor*> HitActors;
};
