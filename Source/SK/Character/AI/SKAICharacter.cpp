#include "Character/AI/SKAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"

ASKAICharacter::ASKAICharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
}

void ASKAICharacter::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributeSetAndAbilitiesFromDataAsset();
}
