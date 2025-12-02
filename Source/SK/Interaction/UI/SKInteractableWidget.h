#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "SKInteractableWidget.generated.h"

UCLASS()
class SK_API USKInteractableWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	FORCEINLINE void SetInitialText(const FText& NewText) { InitialText = NewText; };
	
protected:
	
	UPROPERTY()
	FText InitialText;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* DisplayText;
};
