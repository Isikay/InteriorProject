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
	virtual void UpdateStartPoint(const FVector& Start) = 0;
	virtual void UpdateEndPoint(const FVector& End) = 0;
	virtual void UpdateDimensions(float Height, float Thickness) = 0;
	virtual FVector GetStart() const = 0;
	virtual FVector GetEnd() const = 0;
	virtual float GetLength() const = 0;
	virtual FVector GetDirection() const = 0;
	virtual FVector GetCenter() const = 0;
	virtual FVector2D GetLocalPosition(const FVector& WorldLocation) const = 0;
};