// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"

#include "SkillBarSlotWidget.generated.h"

class ASKPlayerCharacter;
class USKWeaponData;
struct FSkillUIMessage;
struct FSwitchLayoutMessage;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class SK_API USkillBarSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UImage* SkillImage1;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillImage2;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillImage3;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillKey1;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillKey2;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillKey3;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY()
	ASKPlayerCharacter* Character;
	
	UPROPERTY()
	const USKWeaponData* CurrentWeaponData;
	
	FSKGameplayMessageListenerHandle WeaponChangeMessageHandle;

	void OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message);

	FSKGameplayMessageListenerHandle SkillUseMessageHandle;

	void OnSkillUseMessageReceived(FGameplayTag Channel, const FSkillUIMessage& Message);

	void Skill1AnimPlay(bool bSuccess);

	void Skill2AnimPlay(bool bSuccess);

	void Skill3AnimPlay(bool bSuccess);
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_Skill1Success;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_Skill1Fail;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_Skill2Success;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_Skill2Fail;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_Skill3Success;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_Skill3Fail;
};
