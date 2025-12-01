#include "SKDoor.h"

ASKDoor::ASKDoor()
{
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("ChestMesh");
	DoorMesh->SetupAttachment(Root);
	DoorMesh->SetIsReplicated(true);
}