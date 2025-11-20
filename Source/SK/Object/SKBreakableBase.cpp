#include "SKBreakableBase.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

ASKBreakableBase::ASKBreakableBase()
{
	GCComponent = CreateDefaultSubobject<UGeometryCollectionComponent>("GeometryCollectionComponent");
}
