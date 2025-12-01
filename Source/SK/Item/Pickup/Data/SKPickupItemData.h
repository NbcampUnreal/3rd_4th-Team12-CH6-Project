#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraSystem.h"
#include "SKPickupItemData.generated.h"

class USKInventoryItemData;

UCLASS()
class USKPickupItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	// 삭제 예정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	FName ItemName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	FName PickupName;
	
	// 줍기 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	TObjectPtr<UParticleSystem> PickedUpEffect;
	
	// 줍기 파라미터
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	int32 PickupParam;
	
	// 줍기 사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	TObjectPtr<USoundBase> PickupSound;

	// 파괴 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	TObjectPtr<UParticleSystem> DestroyEffect;

	// 드랍 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	TObjectPtr<UNiagaraSystem> DropEffect;
};