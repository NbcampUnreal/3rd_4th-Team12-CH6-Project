// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SKAICharacter.h"

#include "Components/WidgetComponent.h"
#include "UI/Monster/MonsterDamageWidget.h"
#include "UI/Monster/MonsterHealthWidget.h"

ASKAICharacter::ASKAICharacter()
{
	HPBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBar"));
	HPBarWidgetComponent->SetupAttachment(RootComponent);
	HPBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarWidgetComponent->SetDrawSize(FVector2D(150, 15));

	DamageTextWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageText"));
	DamageTextWidgetComponent->SetupAttachment(RootComponent);
	DamageTextWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	DamageTextWidgetComponent->SetDrawSize(FVector2D(150, 15));
}

void ASKAICharacter::BeginPlay()
{
	Super::BeginPlay();

	SetDAPlayerStat();

	if (UUserWidget* Widget = HPBarWidgetComponent->GetUserWidgetObject())
	{
		HPBarWidget = Cast<UMonsterHealthWidget>(Widget);
		if (HPBarWidget)
		{
			HPBarWidget->SettingWidget(this);
		}
	}

	if (UUserWidget* Widget = DamageTextWidgetComponent->GetUserWidgetObject())
	{
		DamageTextWidget = Cast<UMonsterDamageWidget>(Widget);
		if (DamageTextWidget)
		{
			DamageTextWidget->SettingWidget(this);
		}
	}
}
