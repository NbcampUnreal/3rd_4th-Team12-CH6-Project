#pragma once

#include "CoreMinimal.h"
#include "SKInventoryItemData.h"
#include "SKMiscItemData.generated.h"

UENUM(BlueprintType)
enum class EMiscItemType : uint8
{
	Quest,
	Currency,
	Key,
	Material
};

UCLASS()
class SK_API USKMiscItemData : public USKInventoryItemData
{
	GENERATED_BODY()

public:
	USKMiscItemData()
	{
		InventoryType = EInventoryItemType::Misc;
	}

	// Misc 타입 플래그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SK|Misc")
	TSet<EMiscItemType> MiscItemFlags;
};
