#include "SKBonfireBase.h"
#include "SKBonfire.h"

ASKBonfireBase::ASKBonfireBase()
{
}

void ASKBonfireBase::OverLapBeginHandle()
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

void ASKBonfireBase::OverLapEndHandle()
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

