// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/PortalActor.h"

#include "Abilities/GameplayAbility.h"
#include "Character/SKPlayerCharacter.h"
#include "Controller/SKPlayerController.h"
#include "GameMode/TownGameMode.h"
#include "GameMode/DungeonGameMode.h"
#include "Utility/SKGameplayMessageSubsystem.h"
#include "Utility/SKGameplayMessageTypes.h"
#include "Utility/SKNativeGameplayTags.h"
#include "InstancedStruct.h"
#include "StructUtils/InstancedStruct.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APortalActor::APortalActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PortalRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PortalRoot"));
	RootComponent = PortalRoot;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("PortalSphere"));
	Sphere->SetupAttachment(PortalRoot);
	Sphere->SetSphereRadius(150.f);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionObjectType(ECC_WorldDynamic);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Sphere->SetGenerateOverlapEvents(true);
	Sphere->OnComponentBeginOverlap.AddDynamic(
		this,
		&APortalActor::OnOverlapBegin
	);
}

void APortalActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("APortalActor::OnOverlapBegin"));
	
	UWorld* World = GetWorld();
	if (!World) return;
	
	ASKPlayerCharacter* Player = Cast<ASKPlayerCharacter>(OtherActor);
	if (!Player) return;

	ASKPlayerController* PC = Cast<ASKPlayerController>(Player->GetController());
	if (!PC) return;

	bool bIsHost = (PC->IsLocalController() && PC->GetNetMode() == NM_ListenServer);
	bool bIsSingle = (PC->GetNetMode() == NM_Standalone);

	if (!bIsHost && !bIsSingle)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Client] host-only."));
		return;
	}
	
	AGameModeBase* GM = World->GetAuthGameMode();
	if (!GM) return;

	if (GM->IsA(ADungeonGameMode::StaticClass())) // 던전 → 마을
	{
		UE_LOG(LogTemp, Error, TEXT("APortalActor::ReturnToTown"));
		PC->ReturnToTown();
	}
	else if (GM->IsA(ATownGameMode::StaticClass())) // 마을 → 던전 선택 UI
	{
		UE_LOG(LogTemp, Error, TEXT("APortalActor::EnterDungeon"));
		UGameInstance* GI = World->GetGameInstance();
		if (!GI) return;

		USKGameplayMessageSubsystem* MessageSubsystem = GI->GetSubsystem<USKGameplayMessageSubsystem>();

		FSwitchLayoutMessage LayoutMessage;
		LayoutMessage.LayoutTag = TAG_UI_Layout_DungeonSelect;
		LayoutMessage.bVisible = true;

		FInstancedStruct InstanceStruct = FInstancedStruct::Make(LayoutMessage);;

		MessageSubsystem->BroadcastMessage_BP(TAG_Message_Channel_SwitchLayout, InstanceStruct);
	}
}
