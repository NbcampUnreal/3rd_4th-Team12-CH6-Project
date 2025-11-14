// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/SKMeleeHitBox.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ASKMeleeHitBox::ASKMeleeHitBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionResponseToAllChannels(ECR_Overlap);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ASKMeleeHitBox::OnOverlap);

	InitialLifeSpan = HitBoxLifeSpan; // 0.2초 생존
	
}

void ASKMeleeHitBox::InitHitbox(AActor* InOwner, const FGameplayCueParameters& CueParams)
{
	OwnerActor = InOwner;
}

// Called when the game starts or when spawned
void ASKMeleeHitBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASKMeleeHitBox::OnOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 BodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (Other == OwnerActor)
		return;


	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other);

	// if (SourceASC && TargetASC)
	// {
	// 	UAbilitySystemBlueprintLibrary::ApplyGameplayEffectToTarget(
	// 		SourceASC,
	// 		TargetASC,
	// 		DamageGE,
	// 		1.0f
	// 	);
	// }

	if (SourceASC && TargetASC && DamageGE)
	{
		// EffectContext 생성
		FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
		EffectContext.AddSourceObject(OwnerActor);
		EffectContext.AddInstigator(OwnerActor, OwnerActor);

		// EffectSpec 생성
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
			DamageGE,
			1.0f,  // Level
			EffectContext
		);

		if (SpecHandle.IsValid())
		{
			// Target에 Effect 적용
			SourceASC->ApplyGameplayEffectSpecToTarget(
				*SpecHandle.Data.Get(),
				TargetASC
			);
		}
	}
	
	
}


