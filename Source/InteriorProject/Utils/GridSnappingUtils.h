#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GridSnappingUtils.generated.h"

/**
 * Utility class for grid and object snapping functionality.
 */
UCLASS()
class INTERIORPROJECT_API UGridSnappingUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Snaps a point to the nearest grid position */
    UFUNCTION(BlueprintCallable, Category = "Grid Snapping")
    static FVector SnapToGrid(const FVector& Location, float GridSize);

    /** Snaps a point to the nearest line if within threshold */
    UFUNCTION(BlueprintCallable, Category = "Grid Snapping")
    static FVector SnapToLine(const FVector& Point, const FVector& LineStart, const FVector& LineEnd, float Threshold);

    /** Snaps a point to the nearest intersection of two lines */
    UFUNCTION(BlueprintCallable, Category = "Grid Snapping")
    static FVector SnapToLinesIntersection(const FVector& Point, 
        const FVector& Line1Start, const FVector& Line1End,
        const FVector& Line2Start, const FVector& Line2End,
        float Threshold);

    /** Finds the intersection point of two lines */
    UFUNCTION(BlueprintCallable, Category = "Grid Snapping")
    static bool FindLinesIntersection(
        const FVector& Line1Start, const FVector& Line1End,
        const FVector& Line2Start, const FVector& Line2End,
        FVector& OutIntersectionPoint);

    /** Checks if a point is near a line */
    UFUNCTION(BlueprintPure, Category = "Grid Snapping")
    static bool IsPointNearLine(const FVector& Point, const FVector& LineStart, const FVector& LineEnd, float Threshold);

    /** Gets the nearest point on a line */
    UFUNCTION(BlueprintPure, Category = "Grid Snapping")
    static FVector GetNearestPointOnLine(const FVector& Point, const FVector& LineStart, const FVector& LineEnd);

    /** Calculates distance from point to line */
    UFUNCTION(BlueprintPure, Category = "Grid Snapping")
    static float GetDistanceToLine(const FVector& Point, const FVector& LineStart, const FVector& LineEnd);

    /** Snaps angle to nearest increment */
    UFUNCTION(BlueprintPure, Category = "Grid Snapping")
    static float SnapAngle(float Angle, float AngleIncrement);

    /** Snaps position using multiple reference points */
    UFUNCTION(BlueprintCallable, Category = "Grid Snapping")
    static FVector SnapToPoints(const FVector& Location, const TArray<FVector>& Points, float Threshold);
};