#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WallGeometry.generated.h"

UINTERFACE(MinimalAPI)
class UWallGeometry : public UInterface
{
	GENERATED_BODY()
};

class INTERIORPROJECT_API IWallGeometry
{
	GENERATED_BODY()

public:
	/** Update wall start point */
	virtual void UpdateStartPoint(const FVector& Start) = 0;
    
	/** Update wall end point */
	virtual void UpdateEndPoint(const FVector& End) = 0;

	virtual FVector GetCornerLocation(bool bIsStartCorner) const = 0;
    
	/** Update wall dimensions */
	virtual void UpdateDimensions(float Height, float Thickness) = 0;
    
	/** Get wall start point */
	virtual FVector GetStart() const = 0;
    
	/** Get wall end point */
	virtual FVector GetEnd() const = 0;
    
	/** Get wall length */
	virtual float GetLength() const = 0;
    
	/** Get wall direction vector */
	virtual FVector GetDirection() const = 0;
    
	/** Get wall center point */
	virtual FVector GetCenter() const = 0;
    
	/** Convert world location to wall local position */
	virtual FVector2D GetLocalPosition(const FVector& WorldLocation) const = 0;
};