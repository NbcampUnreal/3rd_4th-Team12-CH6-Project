#include "Projectile/SKBaseProjectile.h"
#include "Character/AI/SKAICharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Character/SKCharacterBase.h"

ASKBaseProjectile::ASKBaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->InitSphereRadius(15.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Projectile"));
	SphereComponent->OnComponentHit.AddDynamic(this, &ASKBaseProjectile::OnHit);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->UpdatedComponent = SphereComponent; 
	ProjectileMovementComponent->bShouldBounce = false; 
	ProjectileMovementComponent->ProjectileGravityScale = 0.f; // 중력 영향력 (0.0f이면 직선 이동)
	ProjectileMovementComponent->InitialSpeed = InitialSpeed; 
	ProjectileMovementComponent->MaxSpeed = InitialSpeed; 
	ProjectileMovementComponent->SetAutoActivate(false);

	InitialLifeSpan = 3.f; // 일정 시간 후 자동 소멸 (네트워크 환경에서 유용)
	SetReplicates(true);
	SetReplicateMovement(true);
}

void ASKBaseProjectile::LaunchProjectile(const FVector& LaunchDirection)
{
	if (!IsValid(ProjectileMovementComponent))
	{
		return;
	}

	ProjectileMovementComponent->Velocity = LaunchDirection * InitialSpeed;
	ProjectileMovementComponent->Activate();
}

void ASKBaseProjectile::OnHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
	)
{
	// 투사체를 발사한 주체가 자기 자신이라면 (혹은 무시할 액터라면) 처리하지 않습니다.
	if (OtherActor && (OtherActor != this) && GetInstigator() != OtherActor)
	{
		ASKCharacterBase* Player = Cast<ASKCharacterBase>(OtherActor);
		if (!IsValid(Player))
		{
			Destroy();
		}
		else
		{
			ASKAICharacter* AICharacter = Cast<ASKAICharacter>(GetInstigator());
			if (!IsValid(AICharacter))
			{
				Destroy();
				return;
			}

			UAbilitySystemComponent* OwnerASC = AICharacter->GetAbilitySystemComponent();
			if (!OwnerASC)
			{
				Destroy();
				return;
			}
			
			FGameplayEventData EventData;
			EventData.Instigator = AICharacter;
			EventData.Target = OtherActor;
			EventData.EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Hit"));
			EventData.OptionalObject = nullptr;

			OwnerASC->HandleGameplayEvent(EventData.EventTag, &EventData);
		
			Destroy();
		}
	}
}

void ASKBaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
}

void ASKBaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

