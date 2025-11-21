#include "SKChest.h"

ASKChest::ASKChest()
{
	ChestMesh = CreateDefaultSubobject<UStaticMeshComponent>("ChestMesh");
	ChestMesh->SetupAttachment(Root);

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(Root);
}
