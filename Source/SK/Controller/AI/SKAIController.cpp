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

ASKAIController::ASKAIController()
{
	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComp"));

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.0f; // 시야 범위
	SightConfig->LoseSightRadius = 2000.0f; // 시야 상실 범위
	SightConfig->PeripheralVisionAngleDegrees = 180.0f; // 시야각
	SightConfig->SetMaxAge(5.0f); // 자극 최대 기억 시간
	// 감지 주기 설정은?
	// 아래 감지 팀 설정에 따라 AI 시스템이 감지 대상에 대한 목록을 미리 생성하고 이 목록에 있는 액터만 감지 함. // 내부 세부 로직 궁금하넹.
	// 후에 팀ID 할당해서 불필요한 감지대상 제거해서 자원 소모 줄이기.
	SightConfig->DetectionByAffiliation.bDetectEnemies = true; // 적 감지
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false; // 아군 감지
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false; // 중립 감지

	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
	DamageConfig->SetMaxAge(2.0f);
	
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerceptionComponent->ConfigureSense(*DamageConfig);
	AIPerceptionComponent->SetDominantSense(DamageConfig->GetSenseImplementation());
	
	OwningASC = nullptr;
	
	TargetActor = nullptr;

	CachedTeamID = FGenericTeamId::NoTeam;
}

TObjectPtr<AActor> ASKAIController::GetTargetActor() const
{
	return TargetActor;
}

bool ASKAIController::CheckDistance(float AdditionalCapsuleRadiusSum)
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

FVector ASKAIController::GetTargetDirection() const
{
	if (!IsValid(TargetActor))
	{
		return FVector::ZeroVector;
	}

	FVector ToTargetVector = TargetActor->GetActorLocation() - GetCharacter()->GetActorLocation();
	FVector TargetDirection = ToTargetVector.GetSafeNormal();

	return TargetDirection;
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
	}

	/*///// 테스트
	if (!StateTreeAIComponent)
	{
		return;
	}

	ASKAICharacter* AICharacter = Cast<ASKAICharacter>(InPawn);
	if (!IsValid(AICharacter))
	{
		return;
	}

	UStateTree* OwningStateTree = AICharacter->GetStateTreeAsset();
	if (!OwningStateTree)
	{
		return;
	}
	
	StateTreeAIComponent->SetStateTree(OwningStateTree);
	//StateTreeAIComponent->StartLogic();
	*/
	
}

void ASKAIController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(AIPerceptionComponent))
	{
		return;
	}
	
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ASKAIController::OnTargetPerceptionUpdated);

	auto* GS = GetWorld()->GetGameState<ADungeonGameState>();
	if (!GS) return;

	// 상태 변경 이벤트 수신
	GS->OnDungeonMatchStateChanged.AddUObject(this, &ASKAIController::OnDungeonStateChanged);

	// 이미 진행 중일 수도 있음
	if (GS->DungeonState == EDungeonMatchState::Dungeon_InProgress)
	{
		OnDungeonStateChanged(EDungeonMatchState::Dungeon_InProgress);
	}

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
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("인지성공"));

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
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("감지성공"));
	}

	// 최대 기억 시간에 따라 감지된 목록에 있는 액터 활용 가능
}

void ASKAIController::OnDungeonStateChanged(EDungeonMatchState NewState)
{
	if (NewState == EDungeonMatchState::Dungeon_InProgress)
	{
		if (!StateTreeAIComponent)
		{
			return;
		}

		ASKAICharacter* AICharacter = Cast<ASKAICharacter>(GetCharacter());
		if (!IsValid(AICharacter))
		{
			return;
		}

		UStateTree* OwningStateTree = AICharacter->GetStateTreeAsset();
		if (!OwningStateTree)
		{
			return;
		}
	
		StateTreeAIComponent->SetStateTree(OwningStateTree);
		StateTreeAIComponent->StartLogic();
	}
}
