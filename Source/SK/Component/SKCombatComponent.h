// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SKCombatComponent.generated.h"


struct FSKWeaponDataRow;
struct FWeaponDataRow;
class USKWeaponData;

USTRUCT(BlueprintType)
struct FSKComboState
{
	GENERATED_BODY()

	//  Replicated 영역 (서버 관리)
	UPROPERTY()
	FGameplayTag WeaponTag;
	
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
	bool GetIsAttacking();
	void ResetComboState();
	bool CheckMaxComboIndex(bool bLeft = true);
	FGameplayTag GetLeftATKTag() const;
	FGameplayTag GetWeaponTag() const;
	int32 GetComboIndex() const;
	void StopMontage_Local(float InBlendOut);

	UFUNCTION(Server, Reliable)
	void Server_LeftAttackInput();
	UFUNCTION(Server, Reliable)
	void Server_Notify_StopAttackTrace();
	UFUNCTION(Server, Reliable)
	void Server_OnATKEndNotify(bool bLeft);
	UFUNCTION(Client, Reliable)
	void Client_PlayMontage(UAnimMontage* Montage, FName StartSection);
	UFUNCTION(Client, Reliable)
	void Client_StopMontage(float InBlendOut);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopMontage(float InBlendOut);
	UFUNCTION(Server, Reliable)
	void Server_TryActivateGA(const FGameplayTag& Tag);
	
	UFUNCTION(Server, Reliable)
	void Server_IncreaseComboIndex(bool bLeft = true);
	UFUNCTION(Server, Reliable)
	void Server_ResetComboIndex(int32 NewIndex);
	UFUNCTION(Server, Reliable)
	void Server_SetWeaponTag(FGameplayTag NewWeaponTag);


	
	void SetWeaponTag(const FGameplayTag& NewTag);

	void StartTrace();
	void StopTrace();
	void PerformTrace(float DeltaTime);
	
	void ResetCombo(bool bLeft);

	void ClearHitActor();
	void SetIsTraced(bool ArgIsTracing);
	const TArray<AActor*>& GetHitActors();

	void InitializeWeaponSocket(const FSKWeaponDataRow* Row);
	void InitializeWeaponData(const FWeaponDataRow* Row);


	UAnimMontage* GetLeftAttackMontage(int32 Index);
	void SetWeaponMesh(USkeletalMeshComponent* InWeaponMesh);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SK|Weapon")
	TObjectPtr<USKWeaponData> CurrentWeaponData;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void ActivateLeftAttackGA();

	//애니메이션 등등있음
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CapsuleRadius = 80.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CapsultHalfHeight = 20.f;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetComboStateWeaponTag(FGameplayTag NewWeaponTag) { ComboState.WeaponTag = NewWeaponTag; }
	
private:
	
	int32 GetMaxComboIndex(bool bLeft);
	
	UPROPERTY(ReplicatedUsing=OnRep_ComboState)
	FSKComboState ComboState;


	
	TArray<FName> TraceSockets;
	TArray<FVector> PrevSocketLocations;

	int32 MaxLeftComboIndex = 1;
	int32 MaxRightComboIndex = 1;

	bool bIsTracing = false;
	
	UPROPERTY()
	TArray<AActor*> HitActors;

	FVector PrevStart;
	FVector PrevEnd;

	// 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category="SK|Weapon|Trace")
	FName WeaponStartSocket = "Weapon_Socket_Start";

	UPROPERTY(EditDefaultsOnly, Category="SK|Weapon|Trace")
	FName WeaponEndSocket = "Weapon_Socket_End";

	FString FindWeaponTagName();

	UFUNCTION()
	void OnRep_ComboState();
	
};


