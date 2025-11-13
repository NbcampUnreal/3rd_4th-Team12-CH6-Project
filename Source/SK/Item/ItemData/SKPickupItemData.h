#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKPickupItemData.generated.h"

UCLASS()
class SK_API USKPickupItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	// 인벤토리 아이템 데이터 클래스 필요
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	// TSubclassOf<UInventoryItemData> InventoryItemData;

	// 아이템이 메시일 경우
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	TObjectPtr<UStaticMesh> DisplayMesh;

	// 임시 파티클, 에셋에 따라 나이아가라 시스템
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	TObjectPtr<UParticleSystem> DisplayParticle;
	
	// 줍기 사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	TObjectPtr<USoundBase> PickupSound;

	// 줍기 이펙트, 에셋에 따라 나이아가라 시스템
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Pickup")
	TObjectPtr<UParticleSystem> PickedUpEffect;
};