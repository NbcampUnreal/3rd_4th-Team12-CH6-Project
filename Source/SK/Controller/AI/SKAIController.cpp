#include "Controller/AI/SKAIController.h"
#include "GameFramework/Character.h"
#include "Components/StateTreeAIComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ASKAIController::ASKAIController()
{
	StateTreeAIComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAIComp"));

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	
	SightConfig->SightRadius = 1000.0f; // 시야 범위
	SightConfig->LoseSightRadius = 1500.0f; // 시야 상실 범위
	SightConfig->PeripheralVisionAngleDegrees = 60.0f; // 시야각
	SightConfig->SetMaxAge(5.0f); // 자극 최대 기억 시간
	// 감지 주기 설정은?
	// 아래 감지 팀 설정에 따라 AI 시스템이 감지 대상에 대한 목록을 미리 생성하고 이 목록에 있는 액터만 감지 함. // 내부 세부 로직 궁금하넹.
	// 후에 팀ID 할당해서 불필요한 감지대상 제거해서 자원 소모 줄이기.
	SightConfig->DetectionByAffiliation.bDetectEnemies = true; // 적 감지
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true; // 아군 감지
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true; // 중립 감지

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	TargetActor = nullptr;
}

AActor* ASKAIController::GetTargetActor() const
{
	return TargetActor;
}

void ASKAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!StateTreeAIComponent)
	{
		return;
	}
	
	if (!StateTreeAsset)
	{
		return;
	}

	StateTreeAIComponent->SetStateTree(StateTreeAsset);
	//StateTreeAIComponent->StartLogic(); 기본적으로 자동 호출, 에디터 컴포넌트 디테일에서 설정 가능.
}

void ASKAIController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(AIPerceptionComponent))
	{
		return;
	}
	
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ASKAIController::OnTargetPerceptionUpdated);
}

void ASKAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("인지성공"));
	
	ACharacter* PlayerCharacter = Cast<ACharacter>(Actor);
	if ( !(PlayerCharacter && PlayerCharacter->IsPlayerControlled()) )
	{
		return;
	}
	
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		bool bCanSeePlayer = Stimulus.WasSuccessfullySensed(); // 인지 범위에서 벗어났을 때 false
		if (!bCanSeePlayer)
		{
			TargetActor = nullptr;
			return;
		}
		
		TargetActor = Actor;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("감지성공"));
	}

	// 최대 기억 시간에 따라 감지된 목록에 있는 액터 활용 가능
	// 피격에 대한 감각으로 피격 시 행동 추가 가능 // 청각은 굳이 안 쓸 듯.
}

