#include "SKDoubledoor.h"

ASKDoubledoor::ASKDoubledoor()
{
	DoorMeshLeft = CreateDefaultSubobject<UStaticMeshComponent>("DoorMeshLeft");
	DoorMeshLeft->SetupAttachment(Root);
	DoorMeshLeft->SetIsReplicated(true);
	
	DoorMeshRight = CreateDefaultSubobject<UStaticMeshComponent>("DoorMeshRight");
	DoorMeshRight->SetupAttachment(Root);
	DoorMeshRight->SetIsReplicated(true);
}