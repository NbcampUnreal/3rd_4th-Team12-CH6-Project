// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Weapon/SKWeaponData.h"
#include "BattleComponent.generated.h"

struct FSKWeaponDataRow;
struct FWeaponDataRow;
struct FHitResult;

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


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
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

	UFUNCTION(Server, Reliable)
	void Server_Input_Skill_01();
	UFUNCTION(Server, Reliable)
	void Server_Input_Skill_02();
	UFUNCTION(Server, Reliable)
	void Server_Input_Skill_03();

#pragma region Montage

	UAnimMontage* GetLeftATKMontage(int32 Index);
	UAnimMontage* GetSkillMontage(int32 Index);
	UAnimMontage* GetRightATKMontage(int32 Index);
#pragma endregion

#pragma region AttackTrace
	void StartTrace();
	void StopTrace();
	void PerformTrace(float DeltaTime);

	void ClearHitActor();
	void ClearHitResult();
	void AddHitResult(const FHitResult& Hit);
	void SetIsTraced(bool ArgIsTracing);
	const TArray<AActor*>& GetHitActors();
	const TArray<FHitResult>& GetHitResult();

	UFUNCTION(Server, Reliable)
	void Server_StartTrace();
	UFUNCTION(Server, Reliable)
	void Server_StopTrace();

	UFUNCTION(Server, Reliable)
	void Server_LeftATK_ApplyDamage();
	UFUNCTION(Server, Reliable)
	void Server_ATKTYPE_ApplyDamage(const FGameplayTag& AttackTag);


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

	UPROPERTY()
	TArray<FHitResult> HitResults;

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
