#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GridSnappingUtils.generated.h"

UCLASS()
class INTERIORPROJECT_API UGridSnappingUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Snaps a point to the nearest grid position */
    static FVector SnapToGrid(const FVector& Location, float GridSize)
    {
        return FVector(
            FMath::GridSnap(Location.X, GridSize),
            FMath::GridSnap(Location.Y, GridSize),
            Location.Z
        );
    }

    /** Snaps a point to the nearest line if within threshold */
    static FVector SnapToLine(const FVector& Point, const FVector& LineStart, const FVector& LineEnd, float Threshold)
    {
        FVector LineDir = (LineEnd - LineStart).GetSafeNormal();
        FVector PointToStart = Point - LineStart;
        
        // Project point onto line
        float Dot = FVector::DotProduct(PointToStart, LineDir);
        FVector ProjectedPoint = LineStart + LineDir * Dot;
        
        // Check if projected point is within line segment
        float LineLength = (LineEnd - LineStart).Size();
        if (Dot < 0 || Dot > LineLength)
        {
            return Point;
        }
        
        // Check if point is within threshold distance
        float Distance = FVector::Distance(Point, ProjectedPoint);
        if (Distance <= Threshold)
        {
            return ProjectedPoint;
        }
        
        return Point;
    }

    /** Snaps a point to the nearest intersection if within threshold */
    static FVector SnapToIntersection(const FVector& Point, const TArray<FVector>& Points, float GridSize, float Threshold)
    {
        FVector SnappedPoint = Point;
        float MinDistance = Threshold;

        for (const FVector& GridPoint : Points)
        {
            float Distance = FVector::Distance(Point, GridPoint);
            if (Distance < MinDistance)
            {
                MinDistance = Distance;
                SnappedPoint = GridPoint;
            }
        }

        return SnappedPoint;
    }
};