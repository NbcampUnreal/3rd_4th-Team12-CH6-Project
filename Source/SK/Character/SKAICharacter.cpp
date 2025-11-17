#include "Character/SKAICharacter.h"
#include "Components/CapsuleComponent.h"

ASKAICharacter::ASKAICharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
}

void ASKAICharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeAttributeSetFromDataAsset();
}
