#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WallVisual.generated.h"

UINTERFACE(MinimalAPI)
class UWallVisual : public UInterface
{
	GENERATED_BODY()
};

class INTERIORPROJECT_API IWallVisual
{
	GENERATED_BODY()

public:
	virtual void UpdateMeshGeometry() = 0;
	virtual void UpdateMaterials() = 0;
	virtual void UpdateWallMaterial(UMaterialInterface* Material) = 0; 
};