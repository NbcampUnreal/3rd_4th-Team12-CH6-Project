// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "ItemAddInfoListWidget.generated.h"

class UInventoryComponent;
class UItemAddInfoWidget;
struct FItemAddMessage;
/**
 * 
 */
UCLASS()
class SK_API UItemAddInfoListWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void TryInventoryComponent();

	UFUNCTION()
	void InitializePopupPool();

	UFUNCTION()
	void ProcessPendingMessages();
	
	UFUNCTION()
	UItemAddInfoWidget* GetWidgetPool();
	
	UPROPERTY()
	TArray<UItemAddInfoWidget*> ItemPopupPool;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UItemAddInfoWidget> ItemPopupWidget;
	
	TQueue<FItemAddMessage> PendingMessages;

	FTimerHandle RetryTimerHandle;
	
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* PopupContainer;
	
	UPROPERTY()
	UInventoryComponent* CachedInventory;
	
	FSKGameplayMessageListenerHandle ItemAddHandle;

	void OnAddItemMessageReceived(FGameplayTag Channel, const FItemAddMessage& Message);
};
