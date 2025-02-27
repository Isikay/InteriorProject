// SnappableObject.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SnappableObject.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USnappableObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for objects that can participate in snapping system
 */
class INTERIORPROJECT_API ISnappableObject
{
	GENERATED_BODY()

public:
	/** Get points that other objects can snap to */
	virtual TArray<FVector> GetSnapPoints() const = 0;
    
	/** Get lines that other objects can snap to (as start and end points) */
	virtual TArray<FVector2D> GetSnapLines() const = 0;
    
	/** Determine if the object can snap to a given point */
	virtual bool CanSnapToPoint(const FVector& Point) const = 0;
    
	/** Check if snapping is currently enabled for this object */
	virtual bool IsSnappingEnabled() const = 0;
};