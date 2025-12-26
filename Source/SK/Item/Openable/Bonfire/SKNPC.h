#pragma once

#include "CoreMinimal.h"
#include "Item/Openable/SKOpenableBase.h"
#include "SKNPC.generated.h"

class ASKBonfire;

UCLASS()
class SK_API ASKNPC : public ASKOpenableBase
{
	GENERATED_BODY()

public:
	ASKNPC();

protected:
	virtual void BeginPlay() override;

#pragma region Interaction
	
public:
	UPROPERTY()
	ASKBonfire* OwnerBonfire;

protected:
	virtual void OverLapBeginHandle() override;

	virtual void OverLapEndHandle() override;
	
#pragma endregion
};
