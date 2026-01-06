// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "GameDirectionSlotWidget.generated.h"

class UTextBlock;
class ASKPlayerState;
struct FGameDirectionMessage;

USTRUCT(BlueprintType)
struct FDirectionTextMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value;  // 정확한 GameDirection 값

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
	FText DisplayText;
};


/**
 * 
 */
UCLASS()
class SK_API UGameDirectionSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FSKGameplayMessageListenerHandle GameDirectionHandle;

	void OnGameDirectionMessageReceived(FGameplayTag Channel, const FGameDirectionMessage& Message);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void TryBind();

	void UpdateDirectionText(int32 NewDirection);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ASKPlayerState* CurrentPS;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Direction")
	TArray<FDirectionTextMapping> DirectionTextMappings;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ChangeDirectionAnim;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DirectionText;
};
