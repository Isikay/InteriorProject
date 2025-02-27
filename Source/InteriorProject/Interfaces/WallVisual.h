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
	/** Update wall mesh geometry */
	virtual void UpdateMeshGeometry() = 0;
    
	/** Update wall materials based on state */
	virtual void UpdateMaterials() = 0;
    
	/** Update wall material directly */
	virtual void UpdateWallMaterial(UMaterialInterface* Material) = 0;
};