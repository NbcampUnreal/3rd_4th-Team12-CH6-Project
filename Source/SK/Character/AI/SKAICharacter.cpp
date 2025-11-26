#include "Character/AI/SKAICharacter.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"

ASKAICharacter::ASKAICharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
}

void ASKAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributeSetAndAbilitiesFromDataAsset();
}

void ASKAICharacter::BeginPlay()
{
	Super::BeginPlay();

}
