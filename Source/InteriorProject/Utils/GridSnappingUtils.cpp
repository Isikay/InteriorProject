#include "GridSnappingUtils.h"

FVector UGridSnappingUtils::SnapToGrid(const FVector& Location, float GridSize)
{
    if (GridSize <= 0.0f)
        return Location;

    return FVector(
        FMath::GridSnap(Location.X, GridSize),
        FMath::GridSnap(Location.Y, GridSize),
        Location.Z
    );
}

FVector UGridSnappingUtils::SnapToLine(const FVector& Point, const FVector& LineStart, const FVector& LineEnd, float Threshold)
{
    // Get nearest point on line
    FVector NearestPoint = GetNearestPointOnLine(Point, LineStart, LineEnd);
    
    // Check if point is within threshold distance
    float Distance = FVector::Distance(Point, NearestPoint);
    if (Distance <= Threshold)
    {
        return NearestPoint;
    }
    
    return Point;
}

FVector UGridSnappingUtils::SnapToLinesIntersection(const FVector& Point,
    const FVector& Line1Start, const FVector& Line1End,
    const FVector& Line2Start, const FVector& Line2End,
    float Threshold)
{
    FVector IntersectionPoint;
    if (FindLinesIntersection(Line1Start, Line1End, Line2Start, Line2End, IntersectionPoint))
    {
        float Distance = FVector::Distance(Point, IntersectionPoint);
        if (Distance <= Threshold)
        {
            return IntersectionPoint;
        }
    }
    
    return Point;
}

bool UGridSnappingUtils::FindLinesIntersection(
    const FVector& Line1Start, const FVector& Line1End,
    const FVector& Line2Start, const FVector& Line2End,
    FVector& OutIntersectionPoint)
{
    // Convert to 2D for intersection calculation
    FVector2D P1(Line1Start.X, Line1Start.Y);
    FVector2D P2(Line1End.X, Line1End.Y);
    FVector2D P3(Line2Start.X, Line2Start.Y);
    FVector2D P4(Line2End.X, Line2End.Y);

    float Denominator = (P1.X - P2.X) * (P3.Y - P4.Y) - (P1.Y - P2.Y) * (P3.X - P4.X);
    
    // Check if lines are parallel
    if (FMath::IsNearlyZero(Denominator))
        return false;

    float T = ((P1.X - P3.X) * (P3.Y - P4.Y) - (P1.Y - P3.Y) * (P3.X - P4.X)) / Denominator;
    float U = -((P1.X - P2.X) * (P1.Y - P3.Y) - (P1.Y - P2.Y) * (P1.X - P3.X)) / Denominator;

    // Check if intersection point lies on both line segments
    if (T >= 0.0f && T <= 1.0f && U >= 0.0f && U <= 1.0f)
    {
        OutIntersectionPoint = FVector(
            P1.X + T * (P2.X - P1.X),
            P1.Y + T * (P2.Y - P1.Y),
            Line1Start.Z
        );
        return true;
    }

    return false;
}

bool UGridSnappingUtils::IsPointNearLine(const FVector& Point, const FVector& LineStart, const FVector& LineEnd, float Threshold)
{
    float Distance = GetDistanceToLine(Point, LineStart, LineEnd);
    return Distance <= Threshold;
}

FVector UGridSnappingUtils::GetNearestPointOnLine(const FVector& Point, const FVector& LineStart, const FVector& LineEnd)
{
    FVector LineDirection = (LineEnd - LineStart).GetSafeNormal();
    FVector PointToStart = Point - LineStart;

    float DotProduct = FVector::DotProduct(PointToStart, LineDirection);
    float LineLength = FVector::Distance(LineStart, LineEnd);

    // Clamp projection to line segment
    DotProduct = FMath::Clamp(DotProduct, 0.0f, LineLength);

    return LineStart + LineDirection * DotProduct;
}

float UGridSnappingUtils::GetDistanceToLine(const FVector& Point, const FVector& LineStart, const FVector& LineEnd)
{
    FVector NearestPoint = GetNearestPointOnLine(Point, LineStart, LineEnd);
    return FVector::Distance(Point, NearestPoint);
}

float UGridSnappingUtils::SnapAngle(float Angle, float AngleIncrement)
{
    if (AngleIncrement <= 0.0f)
        return Angle;

    return FMath::GridSnap(Angle, AngleIncrement);
}

FVector UGridSnappingUtils::SnapToPoints(const FVector& Location, const TArray<FVector>& Points, float Threshold)
{
    float MinDistance = Threshold;
    FVector SnappedLocation = Location;

    for (const FVector& Point : Points)
    {
        float Distance = FVector::Distance(Location, Point);
        if (Distance < MinDistance)
        {
            MinDistance = Distance;
            SnappedLocation = Point;
        }
    }

    return SnappedLocation;
}