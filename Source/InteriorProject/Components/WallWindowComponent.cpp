#include "WallWindowComponent.h"
#include "WallGeometryComponent.h"
#include "InteriorProject/WallActor.h"
#include "InteriorProject/Utils/WallMeshGenerator.h"

UWallWindowComponent::UWallWindowComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    WallOwner = nullptr;
    GeometryComponent = nullptr;
}

void UWallWindowComponent::OnInitialize()
{
    GeometryComponent = WallOwner->FindComponentByClass<UWallGeometryComponent>();
}

void UWallWindowComponent::OnCleanup()
{
    Windows.Empty();
    GeometryComponent = nullptr;
}

bool UWallWindowComponent::AddWindow(const FVector2D& Position, const FVector2D& Size, bool bIsBack)
{
    if (!ValidateWindowPlacement(Position, Size))
        return false;

    FWallWindow NewWindow;
    NewWindow.Position = Position;
    NewWindow.Size = Size;
    NewWindow.bIsBackFace = bIsBack;
    
    Windows.Add(NewWindow);
    NotifyWindowsModified();
    
    return true;
}

void UWallWindowComponent::RemoveWindow(int32 WindowIndex)
{
    if (Windows.IsValidIndex(WindowIndex))
    {
        Windows.RemoveAt(WindowIndex);
        NotifyWindowsModified();
    }
}

void UWallWindowComponent::ModifyWindow(int32 WindowIndex, const FVector2D& NewPosition, const FVector2D& NewSize)
{
    if (!Windows.IsValidIndex(WindowIndex))
        return;

    // Create a temporary array without the window being modified
    TArray<FWallWindow> TempWindows = Windows;
    TempWindows.RemoveAt(WindowIndex);

    // Validate new position and size
    if (ValidateWindowPlacement(NewPosition, NewSize))
    {
        Windows[WindowIndex].Position = NewPosition;
        Windows[WindowIndex].Size = NewSize;
        NotifyWindowsModified();
    }
}

void UWallWindowComponent::ClearAllWindows()
{
    if (Windows.Num() > 0)
    {
        Windows.Empty();
        NotifyWindowsModified();
    }
}

bool UWallWindowComponent::ValidateWindowPlacement(const FVector2D& Position, const FVector2D& Size) const
{
    if (!GeometryComponent)
        return false;

    // Check basic constraints first
    if (!ValidateWindowConstraints(Position, Size))
        return false;

    // Create a temporary array with all existing windows
    TArray<FWallWindow> ExistingWindows = Windows;

    // Check overlap with existing windows
    for (const FWallWindow& Window : ExistingWindows)
    {
        // Calculate overlap boxes
        float MinX1 = Position.X;
        float MaxX1 = Position.X + Size.X;
        float MinY1 = Position.Y;
        float MaxY1 = Position.Y + Size.Y;

        float MinX2 = Window.Position.X;
        float MaxX2 = Window.Position.X + Window.Size.X;
        float MinY2 = Window.Position.Y;
        float MaxY2 = Window.Position.Y + Window.Size.Y;

        // Check for overlap with minimum spacing
        if (MaxX1 + MinWindowSpacing > MinX2 && MinX1 - MinWindowSpacing < MaxX2 &&
            MaxY1 + MinWindowSpacing > MinY2 && MinY1 - MinWindowSpacing < MaxY2)
        {
            return false;
        }
    }

    return true;
}

bool UWallWindowComponent::ValidateWindowConstraints(const FVector2D& Position, const FVector2D& Size) const
{
    if (!GeometryComponent)
        return false;

    float WallLength = GeometryComponent->GetLength();
    float WallHeight = GeometryComponent->WallHeight;

    // Check wall bounds with minimum edge distance
    if (Position.X < MinEdgeDistance || 
        Position.X + Size.X > WallLength - MinEdgeDistance ||
        Position.Y < MinEdgeDistance || 
        Position.Y + Size.Y > WallHeight - MinEdgeDistance)
    {
        return false;
    }

    return true;
}

bool UWallWindowComponent::TryPlaceWindowAtLocation(const FVector& WorldLocation)
{
    if (!GeometryComponent)
        return false;

    // Convert world location to wall space
    FVector2D LocalPos = GeometryComponent->GetLocalPosition(WorldLocation);
    
    // Use default window size
    FVector2D DefaultSize(100.0f, 150.0f);

    // Attempt to add window at the calculated position
    return AddWindow(LocalPos, DefaultSize);
}

void UWallWindowComponent::NotifyWindowsModified()
{
    OnWindowsModified.Broadcast();
}