#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Item/SKInteractableBase.h"
#include "SKPickupItem.generated.h"

class UWidgetComponent;
class USphereComponent;
class USKPickupItemData;

USTRUCT(BlueprintType)
struct FSKItemInfo
{
	GENERATED_BODY()
	
	// 아이템 ID
	UPROPERTY(EditAnywhere, Category = "SK|Pickup")
	int32 ItemID;
	
	/** 드랍된 아이템 개수 */
	UPROPERTY(EditAnywhere, Category="SK|Pickup")
	int32 ItemCount = 1;
};

UCLASS()
class SK_API ASKPickupItem : public ASKInteractableBase
{
	GENERATED_BODY()

public:
	ASKPickupItem();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void InitializePickup(int32 ItemID, USKPickupItemData* InPickupData, int32 Count);

protected:
	virtual void BeginPlay() override;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	// TObjectPtr<UParticleSystemComponent> Particle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UNiagaraComponent> ItemNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SK|Data", ReplicatedUsing=OnRep_PickupData)
	TObjectPtr<USKPickupItemData> PickupData;

	USoundBase* GetPickupSound() const;

	int32 GetItemID() const;

	UFUNCTION()
	void OnRep_PickupData();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayPickupEffects(AActor* Interactor);
	
public:
	
	virtual void Interact_Implementation(AActor* Interactor) override;

	FORCEINLINE void SetItemInfo(const int32 NewItemID, const float NewItemCount) { ItemInfo.ItemID = NewItemID; ItemInfo.ItemCount = NewItemCount; }
	FORCEINLINE FSKItemInfo GetItemInfo() const { return ItemInfo; }
	
protected:
	// 아이템 ID
	UPROPERTY(Replicated, EditAnywhere, Category = "SK|Pickup")
	FSKItemInfo ItemInfo;
};