// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmRequestWidgetBase.generated.h"

class USKUIManagerSubSystem;
/**
 * 
 */
UCLASS()
class SK_API UConfirmRequestWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle ConfirmRequestData;

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable, Category = "ConfirmRequest")
	void ConfirmRequest();

	UFUNCTION(BlueprintCallable, Category = "ConfirmRequest")
	virtual void HandleRequestResult(bool bResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USKUIManagerSubSystem* CurrentUISubSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsRequestConfirm;
};
