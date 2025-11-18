#include "SKInteractableBase.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

ASKInteractableBase::ASKInteractableBase()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	TraceCollision = CreateDefaultSubobject<USphereComponent>("TraceCollision");
	TraceCollision->SetupAttachment(Root);

	TraceCollision->SetSphereRadius(100.0f);
	TraceCollision->SetHiddenInGame(false);
	// 전용 트레이스 채널 추가 필요
	TraceCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	InteractionPoint = CreateDefaultSubobject<USceneComponent>("InteractionPoint");
	InteractionPoint->SetupAttachment(Root);
	
	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidgetComponent"));
	InteractionWidgetComponent->SetupAttachment(RootComponent);
	InteractionWidgetComponent->SetVisibility(false);
}

void ASKInteractableBase::BeginPlay()
{
	Super::BeginPlay();

	InteractionData.InteractionLocation = InteractionPoint->GetComponentLocation();
	InteractionData.InteractionRotation = InteractionPoint->GetComponentRotation();
}

void ASKInteractableBase::AddToInventory(AActor* Interactor)
{
	// 플레이어 스테이트 가져오기 및 캐스팅
	// 인벤토리 컴포넌트 가져오기 및 캐스팅

	// 인벤토리 전달용 데이터 구성
	
	// 인벤토리 내부 추가하는 함수 호춯
}

void ASKInteractableBase::OnShowWidget()
{
	// 위젯 표시
}

void ASKInteractableBase::OnHideWidget()
{
	// 위젯 숨김
}

UWidgetComponent* ASKInteractableBase::GetInteractionWidgetComponent() const
{
	return InteractionWidgetComponent;
}


void ASKInteractableBase::GetInteractionData_Implementation(FSKInteractionData& OutData)
{
	OutData = InteractionData;
}
