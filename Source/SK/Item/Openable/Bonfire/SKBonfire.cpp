#include "SKBonfire.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Character/SKPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "NavModifierComponent.h"
#include "Utility/SpawnSubsystem.h"

ASKBonfire::ASKBonfire()
	: bIsDefaultBonfire(false)
{
	BonfireMesh = CreateDefaultSubobject<UStaticMeshComponent>("Bonfire");
	BonfireMesh->SetupAttachment(Root);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NiagaraComponent->SetupAttachment(BonfireMesh);
	NiagaraComponent->SetRelativeScale3D(FVector(2.0f));
	NiagaraComponent->SetRelativeLocation(FVector(0.f, 0.f, 60.0f));

	DetectCollision->SetSphereRadius(1200.f);

	NavModifierComponent = CreateDefaultSubobject<UNavModifierComponent>("NavModifier");
}

void ASKBonfire::BeginPlay()
{
	Super::BeginPlay();

	if (DetectWidget)
	{
		DetectWidgets.Add(DetectWidget);
	}
}


void ASKBonfire::ResetBonfire(ASKPlayerCharacter* PlayerCharacter)
{
	// 캐릭터 스텟 초기화
	if (!PlayerCharacter) return;
	
	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!ASC) return;

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();

	if (ResetGameplayEffect)
	{
		ASC->ApplyGameplayEffectToSelf(ResetGameplayEffect.GetDefaultObject(), 1.0f, Context);
	}

	// 몬스터 재스폰
	auto* SpawnSubSystem = GetWorld()->GetSubsystem<USpawnSubsystem>();
	if (!IsValid(SpawnSubSystem)) return;

	SpawnSubSystem->RespawnAll();
}
