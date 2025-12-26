#include "Character/AI/SKAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "GameAbilitySystem/Attribute/AI/SKAIAttributeSet.h"
#include "UI/Monster/MonsterDamageWidget.h"
#include "UI/Monster/MonsterHealthWidget.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"

ASKAICharacter::ASKAICharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	
	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidgetComponent->SetupAttachment(RootComponent);
	HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::World); // 화면 고정형
	HealthWidgetComponent->SetDrawSize(FVector2D(200.f, 10.f));

	DamageWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidget"));
	DamageWidgetComponent->SetupAttachment(RootComponent);
	DamageWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	DamageWidgetComponent->SetDrawSize(FVector2D(200.f, 50.f));
}

void ASKAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributeSetAndAbilitiesFromDataAsset();
	
	/* 보스한테 추가할 로직
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick([this]()
		{
			if (UWorld* InnerWorld = GetWorld())
			{
				if (USKGameplayMessageSubsystem* MessageSubsystem = USKGameplayMessageSubsystem::Get(InnerWorld))
				{
					FText BossName = FText::FromString(TEXT("테스트 메시지 보스 이름 텍스트"));
					FSettingBossHPWidget Message(this, BossName);

					MessageSubsystem->BroadcastMessage(TAG_Message_Channel_BossWidgetSetting, Message);
					
					FSlotVisibilityMessage SlotMessage;
					SlotMessage.LayoutTag = TAG_UI_Layout_InGame;
					SlotMessage.SlotTags.AddTag(TAG_UI_Slot_BossHP);
					SlotMessage.bVisible = true;

					MessageSubsystem->BroadcastMessage(TAG_Message_Channel_SlotVisible, SlotMessage);

				}
			}
		});
	}
	*/
	AttributeSet->OnCurrentHealthChanged.AddUObject(this, &ASKAICharacter::OnMonsterHealthChange);
}

void ASKAICharacter::BeginPlay()
{
	Super::BeginPlay();

	float Z = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector NewLocation = GetActorLocation();
	NewLocation.Z += Z;
	
	if (HealthWidgetComponent)
	{
		UUserWidget* HealthWidget = HealthWidgetComponent->GetUserWidgetObject();
		if (HealthWidget)
		{
			UMonsterHealthWidget* MonsterHealth = Cast<UMonsterHealthWidget>(HealthWidget);
			if (MonsterHealth)
			{
				MonsterHealth->SettingWidget(this);

				HealthWidgetComponent->SetWorldLocation(NewLocation);
			}
		}
	}

	if (DamageWidgetComponent)
	{
		UUserWidget* DamageWidget = DamageWidgetComponent->GetUserWidgetObject();
		if (DamageWidget)
		{
			UMonsterDamageWidget* MonsterDamage = Cast<UMonsterDamageWidget>(DamageWidget);
			if (MonsterDamage)
			{
				MonsterDamage->SettingWidget(this);

				NewLocation.Z += 30.f;
				DamageWidgetComponent->SetWorldLocation(NewLocation);
			}
		}
	}
}

void ASKAICharacter::OnMonsterHealthChange(AActor* EffectInstigator, AActor* EffectCauser,
	const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	if (UWorld* World = GetWorld())
	{
		// 이미 타이머가 돌고 있다면 제거
		World->GetTimerManager().ClearTimer(WidgetRotationTimerHandle);
		World->GetTimerManager().ClearTimer(WidgetRotationStopTimerHandle);

		// 0.1초마다 실행
		World->GetTimerManager().SetTimer(
			WidgetRotationTimerHandle,
			this,
			&ASKAICharacter::UIWidgetComponentRotationChange,
			0.1f,
			true
		);

		// 1초 후 타이머 종료
		World->GetTimerManager().SetTimer(
			WidgetRotationStopTimerHandle,
			FTimerDelegate::CreateLambda([this, World]()
			{
				World->GetTimerManager().ClearTimer(WidgetRotationTimerHandle);
			}),
			3.0f,
			false
		);
	}
}

void ASKAICharacter::UIWidgetComponentRotationChange()
{
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	APlayerCameraManager* CamManager = PC->PlayerCameraManager;
	if (!CamManager) return;

	FVector CameraLocation = CamManager->GetCameraLocation();

	auto RotateToCamera = [&](UWidgetComponent* WidgetComp)
	{
		if (!WidgetComp) return;

		FVector WidgetLocation = WidgetComp->GetComponentLocation();

		// 카메라 → 위젯 방향
		FVector Direction = CameraLocation - WidgetLocation;

		// 위/아래로 기울어지는 것 방지
		Direction.Z = 0.f;

		FRotator LookRot = Direction.Rotation();

		WidgetComp->SetWorldRotation(LookRot);
	};

	RotateToCamera(HealthWidgetComponent);
	RotateToCamera(DamageWidgetComponent);
}
