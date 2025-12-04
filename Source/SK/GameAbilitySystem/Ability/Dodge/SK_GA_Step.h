#pragma once

#include "CoreMinimal.h"
#include "SK_GA_Dodge.h"
#include "SK_GA_Step.generated.h"

class USKActionComponent;

UCLASS()
class SK_API USK_GA_Step : public USK_GA_Dodge
{
	GENERATED_BODY()

protected:
	virtual void PreActivateDodge(USKActionComponent* ActionComponent) override;
};
