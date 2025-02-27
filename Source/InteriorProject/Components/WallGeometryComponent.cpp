#include "WallGeometryComponent.h"

UWallGeometryComponent::UWallGeometryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    WallStart = FVector::ZeroVector;
    WallEnd = FVector::ZeroVector;
}

void UWallGeometryComponent::UpdateStartPoint(const FVector& Start)
{
    if (WallStart != Start)
    {
        WallStart = Start;
        OnGeometryChanged.Broadcast();
    }
}

void UWallGeometryComponent::UpdateEndPoint(const FVector& End)
{
    if (WallEnd != End)
    {
        WallEnd = End;
        OnGeometryChanged.Broadcast();
    }
}

void UWallGeometryComponent::UpdateDimensions(float Height, float Thickness)
{
    bool bChanged = false;
    
    if (!FMath::IsNearlyEqual(WallHeight, Height))
    {
        WallHeight = Height;
        bChanged = true;
    }

    if (!FMath::IsNearlyEqual(WallThickness, Thickness))
    {
        WallThickness = Thickness;
        bChanged = true;
    }

    if (bChanged)
    {
        OnGeometryChanged.Broadcast();
    }
}

FVector UWallGeometryComponent::GetCornerLocation(bool bIsStartCorner) const
{
    return bIsStartCorner ? WallStart : WallEnd;
}

float UWallGeometryComponent::GetLength() const
{
    return (WallEnd - WallStart).Size();
}

FVector UWallGeometryComponent::GetDirection() const
{
    return (WallEnd - WallStart).GetSafeNormal();
}

FVector UWallGeometryComponent::GetCenter() const
{
    return (WallStart + WallEnd) * 0.5f;
}

FVector2D UWallGeometryComponent::GetLocalPosition(const FVector& WorldLocation) const
{
    // Get wall direction and up vector
    FVector WallDir = GetDirection();
    FVector UpVector(0, 0, 1);

    // Calculate relative position
    FVector RelativePos = WorldLocation - WallStart;

    // Project onto wall direction for X coordinate
    float X = FVector::DotProduct(RelativePos, WallDir);
    
    // Use Z coordinate directly for Y (height)
    float Y = RelativePos.Z;

    return FVector2D(X, Y);
}

void UWallGeometryComponent::UpdateCornerLocation(bool bIsStartCorner, const FVector& Vector)
{
    if (bIsStartCorner)
    {
        UpdateStartPoint(Vector);
    }
    else
    {
        UpdateEndPoint(Vector);
    }
}