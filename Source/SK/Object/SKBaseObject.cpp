// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/SKBaseObject.h"

#include "AbilitySystemComponent.h"
#include "Character/SKCharacterBase.h"
#include "Components/SphereComponent.h"
#include "GameMode/SKGameMode.h"

// Sets default values
ASKBaseObject::ASKBaseObject()
{
	DefaultConstructor();
}

// Called when the game starts or when spawned
void ASKBaseObject::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASKBaseObject::OnOverlapBegin);
	SetAllowedTeamTag();
}

void ASKBaseObject::DefaultConstructor()
{
	bReplicates = true;
	SetReplicateMovement(true);

	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(FName("CollisionComponent"));
	CollisionComp->InitSphereRadius(50.f);
	//충돌감지설정 QueryOnly : 충돌감지만하고 물리적충돌X
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//모든충돌채널에 대한 기본반응설정 : ECR_Ignore(모든오브젝트와 충돌무시)
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	//특정 채널에 대한 예외설정 ECC_Pawn(Pawn채널만) , ECR_Overlap(pawn과는 겹침감지)
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollisionComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComp->SetupAttachment(RootComponent);
	//메시는 충돌안함
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool ASKBaseObject::HasAllowedTeamTag(UAbilitySystemComponent* ASC) const
{
	FGameplayTagContainer AllowedTagContainer;
	for (const FGameplayTag& Tag : AllowedTeamTag)
	{
		AllowedTagContainer.AddTag(Tag);
	}

	// 플레이어가 허용된 팀 태그 중 하나라도 가지고 있는지 확인
	return ASC->HasAnyMatchingGameplayTags(AllowedTagContainer);
}


void ASKBaseObject::OnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	// Cosmetic 전용이면 시각 효과만 처리하고 파괴
	if (bIsCosmetic)
	{
		ASKCharacterBase* Player = Cast<ASKCharacterBase>(OtherActor);
		if (Player)
		{// 충돌 감지 시 시각적으로만 사라짐
			UE_LOG(LogTemp, Log, TEXT("[ATFDBaseObject][OnOverlapBegin] Cosmetic Projectile Hit detected, destroying visual"));
			Destroy();
		}
		return;
	}

	if (!CollisionEffect)
		return;

	ASKCharacterBase* Player = Cast<ASKCharacterBase>(OtherActor);
	if (!Player)
		return;

	UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
	if (!ASC)
		return;

	if (!HasAllowedTeamTag(ASC))
		return;

	// 실제 로직 실행
	ApplyEffectAndDestroy(ASC);
	
}

void ASKBaseObject::SetAllowedTeamTag()
{
	ASKGameMode* pGameMode = GetWorld()->GetAuthGameMode<ASKGameMode>();
	if (!pGameMode)
		return;

//	AllowedTeamTag = pGameMode->GetDTAllowedTeamTagContainer(ItemTag);
}

void ASKBaseObject::ApplyEffectAndDestroy(UAbilitySystemComponent* ASC)
{
	// 게임플레이 이펙트 적용
	ASC->ApplyGameplayEffectToSelf(CollisionEffect.GetDefaultObject(), 1.f, ASC->MakeEffectContext());

	// 오브젝트 파괴
	Destroy();
}

// Called every frame
void ASKBaseObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

