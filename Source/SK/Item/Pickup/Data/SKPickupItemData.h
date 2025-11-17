#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKPickupItemData.generated.h"

class UInventoryItemData;

UCLASS()
class USKPickupItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	FName ItemName;
	
	// 아이템 ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	FName ItemID;
	
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
};