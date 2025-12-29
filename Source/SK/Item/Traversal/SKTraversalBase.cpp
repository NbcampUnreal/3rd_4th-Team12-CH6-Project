#include "SKTraversalBase.h"

#include "Components/SphereComponent.h"
#include "GameData/SKGameConstant.h"

ASKTraversalBase::ASKTraversalBase()
{
	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>("ObstacleMesh");
	ObstacleMesh->SetupAttachment(Root);
	// ObstacleMesh->SetCollisionObjectType(SKConstant::ECC_Interactable);
	
	DetectCollision->SetSphereRadius(500.0f);

	ObjectType = EObjectType::Obstacle;
}
