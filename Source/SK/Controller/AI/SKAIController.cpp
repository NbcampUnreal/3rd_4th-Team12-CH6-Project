#include "Controller/AI/SKAIController.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "Character/AI/SKAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameState/DungeonGameState.h"
#include "Perception/AISenseConfig_Damage.h"
#include "PlayerState/SKPlayerState.h"
#include "Utility/ETC/BossTriggerActor.h"
#include "EngineUtils.h"

ASKAIController::ASKAIController()
{
	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComp"));

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2000.0f; // 시야 범위
	SightConfig->LoseSightRadius = 2500.0f; // 시야 상실 범위
	SightConfig->PeripheralVisionAngleDegrees = 180.0f; // 시야각
	SightConfig->SetMaxAge(5.0f); // 자극 최대 기억 시간
	SightConfig->DetectionByAffiliation.bDetectEnemies = true; // 적 감지
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false; // 아군 감지
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false; // 중립 감지

	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
	DamageConfig->SetMaxAge(2.0f);
	
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->ConfigureSense(*DamageConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	
	OwningASC = nullptr;
	
	TargetActor = nullptr;

	CachedTeamID = FGenericTeamId::NoTeam;
}

TObjectPtr<AActor> ASKAIController::GetTargetActor() const
{
	return TargetActor;
}

bool ASKAIController::CheckClose(float AdditionalCapsuleRadiusSum)
{
	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(GetCharacter());
	if (!IsValid(AICharacter))
	{
		return false;
	}

	if (!IsValid(TargetActor))
	{
		return false;
	}
	
	ASKPlayerCharacter* PlayerCharacter = Cast<ASKPlayerCharacter>(TargetActor);
	if (!IsValid(PlayerCharacter))
	{
		return false;
	}

	FVector AILocation = AICharacter->GetActorLocation();
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	
	float CapsuleRadiusSum = AICharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() + PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float Distance = (PlayerLocation - AILocation).Size2D();

	if (Distance <= CapsuleRadiusSum + AdditionalCapsuleRadiusSum)
	{
		return true;
	}
	
	return false;
}

FVector ASKAIController::GetPredictedTargetLocation(float PredictionTime) const
{
	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(GetCharacter());
	if (!IsValid(AICharacter))
	{
		return FVector::ZeroVector;
	}

	FVector AILocation = AICharacter->GetActorLocation();
	
	if (!IsValid(TargetActor))
	{
		return AILocation;
	}

	FVector PlayerLocation = TargetActor->GetActorLocation();
	FVector PlayerVelocity = TargetActor->GetVelocity();
	
	FVector PredictedLocation = PlayerLocation + PlayerVelocity * PredictionTime;
	PredictedLocation.Z = AILocation.Z;

	return PredictedLocation;
}

FVector ASKAIController::GetPredictedToTargetDirection(const FVector& PredictedLocation) const
{
	FVector ToTargetVector = PredictedLocation - GetCharacter()->GetActorLocation();
	FVector ToTargetDirection = ToTargetVector.GetSafeNormal();

	return ToTargetDirection;
}

void ASKAIController::AddTag(FGameplayTag Tag) const
{
	if (!IsValid(OwningASC))
	{
		return;
	}

	OwningASC->AddLooseGameplayTag(Tag);
}

void ASKAIController::RemoveTag(FGameplayTag Tag) const
{
	if (!IsValid(OwningASC))
	{
		return;
	}

	OwningASC->RemoveLooseGameplayTag(Tag);
}

void ASKAIController::SendEventToASC(AActor* LocalInstigator, AActor* LocalTargetActor, FGameplayTag EventTag) const
{
	if (!IsValid(OwningASC))
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.Instigator = LocalInstigator;
	EventData.Target = LocalTargetActor;
	EventData.EventTag = EventTag;
	EventData.OptionalObject = nullptr;

	OwningASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void ASKAIController::FindClosestTarget()
{
	float MinDistanceSquared = FLT_MAX;

	AActor* ClosestTarget = nullptr;

	ACharacter* AI = GetCharacter();
	if (!IsValid(AI))
	{
		return;
	}
	
	FVector AILocation = AI->GetActorLocation();
	
	for (TObjectPtr<AActor> LocalTargetActor : TargetActors)
	{
		if (!IsValid(LocalTargetActor))
		{
			continue;
		}
		
		if (MinDistanceSquared > FVector::DistSquared(AILocation, LocalTargetActor->GetActorLocation()))
		{
			MinDistanceSquared = FVector::DistSquared(AILocation, LocalTargetActor->GetActorLocation());
			ClosestTarget = LocalTargetActor;
		}
	}

	TargetActor = ClosestTarget;
}

uint8 ASKAIController::ConvertTeamTagToID(const FGameplayTagContainer& InTags) const
{
	if (InTags.HasTagExact(FGameplayTag::RequestGameplayTag("Team.Player")))
		return 0;

	if (InTags.HasTagExact(FGameplayTag::RequestGameplayTag("Team.Monster")))
		return 1;

	return FGenericTeamId::NoTeam; // 255
}

uint8 ASKAIController::GetTeamIDFromActor(const AActor& Other) const
{
	const APawn* OtherPawn = Cast<APawn>(&Other);
	if (!OtherPawn)
		return FGenericTeamId::NoTeam;

	const ASKPlayerState* PS = OtherPawn->GetPlayerState<ASKPlayerState>();
	if (!PS)
		return FGenericTeamId::NoTeam;

	return PS->PlayerTeamID.GetId();
}

ETeamAttitude::Type ASKAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const uint8 MyID = CachedTeamID.GetId();
	const uint8 OtherID = GetTeamIDFromActor(Other);

	if (OtherID == FGenericTeamId::NoTeam)
		return ETeamAttitude::Neutral;

	if (MyID == OtherID)
		return ETeamAttitude::Friendly;

	return ETeamAttitude::Hostile;
}

void ASKAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(InPawn);
	if (!ASCInterface)
	{
		return;
	}

	OwningASC = ASCInterface->GetAbilitySystemComponent();
	if (OwningASC)
	{
		FGameplayTagContainer InTags;
		OwningASC->GetOwnedGameplayTags(InTags);

		uint8 TeamValue = ConvertTeamTagToID(InTags);
		CachedTeamID = FGenericTeamId(TeamValue);

		UE_LOG(LogTemp, Log, TEXT("AI TeamID Set: %d"), TeamValue);

		if (OwningASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Common"))))
		{
			SightConfig->SightRadius = 700.0f;
			SightConfig->LoseSightRadius = 800.0f;

			AIPerceptionComponent->ConfigureSense(*SightConfig);
		}
	}

	auto* GS = GetWorld()->GetGameState<ADungeonGameState>();
	if (!GS) return;

	// 상태 변경 이벤트 수신
	GS->OnDungeonMatchStateChanged.AddUObject(this, &ASKAIController::OnDungeonStateChanged);

	CurrentDungeonState = GS->DungeonState;
	
	CachedAICharacter = Cast<ASKAICharacter>(InPawn);
	if (!CachedAICharacter)	return;

	ActivationPolicy = CachedAICharacter->ActivationPolicy;

	if (ActivationPolicy == EAIActivationPolicy::Triggered)
	{
		for (TActorIterator<ABossTriggerActor> It(GetWorld()); It; ++It)
		{
			It->OnAITriggered.AddUObject(
				this,
				&ASKAIController::OnActivationTriggered
			);
		}
	}

	for (TActorIterator<ABossTriggerActor> It(GetWorld()); It; ++It)
	{
		It->OnAITriggered.AddUObject(
			this,
			&ASKAIController::OnActivationTriggered
		);
	}

	if (CurrentDungeonState == EDungeonMatchState::Dungeon_InProgress && ActivationPolicy == EAIActivationPolicy::Immediate)
	{
		ApplyDungeonState();
	}
}

void ASKAIController::OnUnPossess()
{
	Super::OnUnPossess();

	CachedAICharacter = nullptr;
}

void ASKAIController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(AIPerceptionComponent))
	{
		return;
	}
	
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ASKAIController::OnTargetPerceptionUpdated);

	/*
	auto* GS = GetWorld()->GetGameState<ADungeonGameState>();
	if (!GS) return;

	// 상태 변경 이벤트 수신
	GS->OnDungeonMatchStateChanged.AddUObject(this, &ASKAIController::OnDungeonStateChanged);

	CurrentDungeonState = GS->DungeonState;

	// 이미 진행 중일 수도 있음
	if (CurrentDungeonState == EDungeonMatchState::Dungeon_InProgress)
	{
		//ApplyDungeonState();
	}*/
	
	GetWorld()->GetTimerManager().SetTimer(
	   FindClosestTargetTimerHandle,
	   this,
	   &ASKAIController::FindClosestTarget, 
	   0.5f,
	   true
   );
}

void ASKAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	SendEventToASC(this, TargetActor, FGameplayTag::RequestGameplayTag("Event.MoveComplete"));
}

void ASKAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	ACharacter* PlayerCharacter = Cast<ACharacter>(Actor);
	if ( !(PlayerCharacter && PlayerCharacter->IsPlayerControlled()) )
	{
		return;
	}

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
	{
		TargetActor = Actor;
	}
	
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		bool bCanSeePlayer = Stimulus.WasSuccessfullySensed(); // 인지 범위에서 벗어났을 때 false
		if (!bCanSeePlayer)
		{
			TargetActors.Remove(Actor);
			RemoveTag(FGameplayTag::RequestGameplayTag("AI.Perception"));
			SendEventToASC(this, TargetActor, FGameplayTag::RequestGameplayTag("Event.EndAbility"));
			return;
		}

		TargetActors.Add(Actor);
		AddTag(FGameplayTag::RequestGameplayTag("AI.Perception"));
		SendEventToASC(this, TargetActor, FGameplayTag::RequestGameplayTag("Event.EndAbility"));
	}

	// 최대 기억 시간에 따라 감지된 목록에 있는 액터 활용 가능
}

void ASKAIController::OnDungeonStateChanged(EDungeonMatchState NewState)
{
	CurrentDungeonState = NewState;
	
	if (CurrentDungeonState == EDungeonMatchState::Dungeon_InProgress && ActivationPolicy == EAIActivationPolicy::Immediate)
	{
		ApplyDungeonState();
	}
}

void ASKAIController::OnActivationTriggered()
{
	UE_LOG(LogTemp, Display, TEXT("OnActivationTriggered() Start"));
	if (CurrentDungeonState == EDungeonMatchState::Dungeon_InProgress && ActivationPolicy == EAIActivationPolicy::Triggered)
	{
		ApplyDungeonState();
	}
}

void ASKAIController::ApplyDungeonState()
{
	if (!StateTreeAIComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not StateTreeAIComponent"));
		return;
	}

	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(GetCharacter());
	if (!IsValid(AICharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("Not AICharacter"));
		return;
	}

	UStateTree* OwningStateTree = AICharacter->GetStateTreeAsset();
	if (!OwningStateTree)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not OwningStateTree"));
		return;
	}
	
	StateTreeAIComponent->SetStateTree(OwningStateTree);
	StateTreeAIComponent->StartLogic();
}