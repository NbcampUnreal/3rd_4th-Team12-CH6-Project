#include "SKNPC.h"

#include "Components/SphereComponent.h"
#include "SKBonfire.h"

ASKNPC::ASKNPC()
{
	InteractionCollision->SetSphereRadius(150.0f);
	InteractionCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));

	DetectCollision->SetSphereRadius(300.f);
	
	ObjectType = EObjectType::NPC;
}


void ASKNPC::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerBonfire = Cast<ASKBonfire>(GetAttachParentActor());
}

void ASKNPC::OverLapBeginHandle()
{
	if (OwnerBonfire)
	{
		TSet<UWidgetComponent*>& DetectWidgets = OwnerBonfire->DetectWidgets;
		if (DetectWidget)
		{
			DetectWidgets.Add(DetectWidget);
		}
	}
}

void ASKNPC::OverLapEndHandle()
{
	if (OwnerBonfire)
	{
		TSet<UWidgetComponent*>& DetectWidgets = OwnerBonfire->DetectWidgets;
		if (DetectWidget)
		{
			DetectWidgets.Remove(DetectWidget);
		}
	}
}
