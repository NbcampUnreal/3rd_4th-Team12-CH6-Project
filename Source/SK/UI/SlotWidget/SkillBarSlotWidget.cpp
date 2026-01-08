// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget/SkillBarSlotWidget.h"

#include "Character/SKPlayerCharacter.h"
#include "Component/BattleComponent.h"
#include "Component/SKCombatComponent.h"
#include "Components/Image.h"
#include "GameAbilitySystem/Attribute/SKAttributeSet.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

void USkillBarSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UWorld* World = GetWorld();
	if (!World)
		return;
		
	UGameInstance* GameInstance = World->GetGameInstance();
	if(!GameInstance)
		return;
		
	USKGameplayMessageSubsystem* MessageSubsystem = GameInstance->GetSubsystem<USKGameplayMessageSubsystem>();
	if (!MessageSubsystem)
		return;

	// 메시지 구독
	SkillUseMessageHandle = MessageSubsystem->RegisterListener<FSkillUIMessage>(
		TAG_Message_Channel_SkillUse,
		this,
		&USkillBarSlotWidget::OnSkillUseMessageReceived
	);

	WeaponChangeMessageHandle = MessageSubsystem->RegisterListener<FSwitchLayoutMessage>(
		TAG_Message_Channel_SwitchLayout,
		this,
		&USkillBarSlotWidget::OnSwitchLayoutMessageReceived
	);

	APawn* Pawn = GetOwningPlayerPawn();
	if (!Pawn)
		return;

	Character = Cast<ASKPlayerCharacter>(Pawn);
	if (!Character)
		return;
	
	CurrentWeaponData = Character->GetBattleComponent()->GetCurrentWeaponData();

	APlayerState* Temp = GetOwningPlayerState();
	if (!Temp)
		return;

	ASKPlayerState* PS = Cast<ASKPlayerState>(Temp);
	if (!PS)
		return;
	
	if (Attribute)
	{
		Attribute->OnHeatChanged.RemoveAll(this);
		Attribute = nullptr;
	}
	
	Attribute = PS->GetAttributeSet();

	Attribute->OnHeatChanged.AddUObject(this, &USkillBarSlotWidget::HeatChanged);
	
	CheckSkillIcon();
}

void USkillBarSlotWidget::NativeDestruct()
{
	if (SkillUseMessageHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(SkillUseMessageHandle);
		}
	}

	if (WeaponChangeMessageHandle.IsValid())
	{
		if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(this))
		{
			MessageSubsystem->UnregisterListener(WeaponChangeMessageHandle);
		}
	}
	
	if (Attribute)
	{
		Attribute->OnHeatChanged.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void USkillBarSlotWidget::HeatChanged(AActor* EffectInstigator, AActor* EffectCauser,
	const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	UpdateSkillIcon();
}

void USkillBarSlotWidget::OnSwitchLayoutMessageReceived(FGameplayTag Channel, const FSwitchLayoutMessage& Message)
{
	if (Message.LayoutTag != TAG_UI_Layout_InGame)
	{
		return;
	}
	
	CheckSkillIcon();
}

void USkillBarSlotWidget::OnSkillUseMessageReceived(FGameplayTag Channel, const FSkillUIMessage& Message)
{
	StopAllAnimations();
	
	switch (Message.SkillNum)
	{
	case 0:
		Skill1AnimPlay(Message.bSuccess);
		break;
	case 1:
		Skill2AnimPlay(Message.bSuccess);
		break;
	case 2:
		Skill3AnimPlay(Message.bSuccess);
		break;
	default:
		break;
	}
}

void USkillBarSlotWidget::CheckSkillIcon()
{
	if (!Character)
	{
		if (APawn* Pawn = GetOwningPlayerPawn())
		{
			Character = Cast<ASKPlayerCharacter>(Pawn);
		}
	}

	if (!Character)
		return;

	const USKWeaponData* WeaponData =
		Character->GetBattleComponent()->GetCurrentWeaponData();

	if (!WeaponData)
		return;

	const TArray<TObjectPtr<UTexture2D>>& Icons = WeaponData->SkillImages;

	auto SetSkillIcon = [](UImage* Image, UTexture2D* Texture)
	{
		if (!Image) return;

		if (Texture)
		{
			Image->SetBrushFromTexture(Texture);
			Image->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Image->SetVisibility(ESlateVisibility::Hidden);
		}
	};

	SetSkillIcon(SkillImage1, Icons.IsValidIndex(0) ? Icons[0] : nullptr);
	SetSkillIcon(SkillImage2, Icons.IsValidIndex(1) ? Icons[1] : nullptr);
	SetSkillIcon(SkillImage3, Icons.IsValidIndex(2) ? Icons[2] : nullptr);
}
void USkillBarSlotWidget::Skill1AnimPlay(bool bSuccess)
{
	if (bSuccess)
	{
		PlayAnimation(
			Anim_Skill1Success,
			0.f,
			1,
			EUMGSequencePlayMode::Forward,
			1.0f
		);
	}
	else
	{
		PlayAnimation(
			Anim_Skill1Fail,
			0.f,
			1,
			EUMGSequencePlayMode::Forward,
			1.0f
		);
	}
}

void USkillBarSlotWidget::Skill2AnimPlay(bool bSuccess)
{
	if (bSuccess)
	{
		PlayAnimation(
			Anim_Skill2Success,
			0.f,
			1,
			EUMGSequencePlayMode::Forward,
			1.0f
		);
	}
	else
	{
		PlayAnimation(
			Anim_Skill2Fail,
			0.f,
			1,
			EUMGSequencePlayMode::Forward,
			1.0f
		);
	}
}

void USkillBarSlotWidget::Skill3AnimPlay(bool bSuccess)
{
	if (bSuccess)
	{
		PlayAnimation(
			Anim_Skill3Success,
			0.f,
			1,
			EUMGSequencePlayMode::Forward,
			1.0f
		);
	}
	else
	{
		PlayAnimation(
			Anim_Skill3Fail,
			0.f,
			1,
			EUMGSequencePlayMode::Forward,
			1.0f
		);
	}
}

void USkillBarSlotWidget::UpdateSkillIcon()
{
	if (!Attribute)
		return;

	if (Attribute->GetHeat() <= 0.f)
	{
		SkillImage1->SetRenderOpacity(0.5f);
		SkillImage2->SetRenderOpacity(0.5f);
		SkillImage3->SetRenderOpacity(0.5f);
	}
	else if (Attribute->GetHeat() >= 1.f)
	{
		SkillImage1->SetRenderOpacity(1.0f);
		SkillImage2->SetRenderOpacity(1.0f);
		SkillImage3->SetRenderOpacity(1.0f);
	}
}
