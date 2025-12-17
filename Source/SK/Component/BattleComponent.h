// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Weapon/SKWeaponData.h"
#include "BattleComponent.generated.h"

struct FSKWeaponDataRow;
struct FWeaponDataRow;

USTRUCT(BlueprintType)
struct FSKBattleState
{
	GENERATED_BODY()

	//  Replicated 영역 (서버 관리)
	UPROPERTY()
	FGameplayTag WeaponTag;

	UPROPERTY(NotReplicated)
	bool bBufferedAttack = false;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SK_API UBattleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBattleComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void BeginPlay() override;

#pragma region Montage

	UAnimMontage* GetLeftATKMontage(int32 Index);
	UAnimMontage* GetSkillMontage(int32 Index);
#pragma endregion
	
#pragma region AttackTrace
	void StartTrace();
	void StopTrace();
	void PerformTrace(float DeltaTime);
	
	void ClearHitActor();
	void SetIsTraced(bool ArgIsTracing);
	const TArray<AActor*>& GetHitActors();

	UFUNCTION(Server, Reliable)
	void Server_StartTrace();
	UFUNCTION(Server, Reliable)
	void Server_StopTrace();
	UFUNCTION(Server, Reliable)
	void Server_LeftATK_ApplyDamage();
	UFUNCTION(Server, Reliable)
	void Server_OnATKEndNotify(bool bLeft);
	
#pragma endregion

#pragma region WeaponData
	void SetWeaponMesh(USkeletalMeshComponent* InWeaponMesh);
	void SetWeaponMesh_Init();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Weapon")
	TObjectPtr<USKWeaponData> CurrentWeaponData;
	
#pragma endregion 
	void InitializeWeaponSocket(const FSKWeaponDataRow* Row);
	void InitializeWeaponData(const FWeaponDataRow* Row);
protected:

	FString FindWeaponTagName();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CapsuleRadius = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CapsultHalfHeight = 60.f;
	
private:

	UPROPERTY(ReplicatedUsing=OnRep_ComboState)
	FSKBattleState ComboState;

	UFUNCTION()
	void OnRep_ComboState();
	

#pragma region TraceVariable
	bool bIsTracing = false;

	UPROPERTY()
	TArray<AActor*> HitActors;

	TArray<FName> TraceSockets;
	TArray<FVector> PrevSocketLocations;

	UPROPERTY(EditDefaultsOnly, Category="SK|Weapon|Trace")
	FName WeaponStartSocket = "Weapon_Socket_Start";

	UPROPERTY(EditDefaultsOnly, Category="SK|Weapon|Trace")
	FName WeaponEndSocket = "Weapon_Socket_End";

	FVector PrevStart;
	FVector PrevEnd;
#pragma endregion
	
		
};
