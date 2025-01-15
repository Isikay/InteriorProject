#include "WindowPlacementWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "InteriorProject/WallActor.h"
#include "InteriorProject/Components/WallGeometryComponent.h"
#include "InteriorProject/Components/WallWindowComponent.h"

void UWindowPlacementWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    bIsDragging = false;
    bIsValidPlacement = false;
    InitialSize = FVector2D(100.0f, 150.0f);
    CurrentSize = InitialSize;
    StartPosition = FVector2D::ZeroVector;
    
    if (MainBorder)
    {
        MainBorder->SetBrushColor(DefaultColor);
        
        // Set initial size
        FWidgetTransform Transform;
        Transform.Scale = InitialSize / 100.0f;
        MainBorder->SetRenderTransform(Transform);
    }
}

void UWindowPlacementWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());

    if (!bIsDragging)
    {
        FollowMouse(MousePosition);
    }
    else if (TargetWall)
    {
        FVector2D LocalPos = GetLocalPositionOnWall(MousePosition);
        FVector2D NewSize;
        NewSize.X = FMath::Abs(LocalPos.X - StartPosition.X);
        NewSize.Y = 150.0f; // Fixed height
        
        UpdateWindowSize(NewSize);
        FollowMouse(MousePosition);
    }

    ValidatePlacement();
}

void UWindowPlacementWidget::SetTargetWall(AWallActor* Wall)
{
    CleanupReferences();

    TargetWall = Wall;
    if (TargetWall)
    {
        GeometryComponent = TargetWall->FindComponentByClass<UWallGeometryComponent>();
        WindowComponent = TargetWall->FindComponentByClass<UWallWindowComponent>();
        
        UpdateVisuals();
        
        // Update position immediately
        FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
        FollowMouse(MousePosition);
    }
}

void UWindowPlacementWidget::ClearTargetWall()
{
    CleanupReferences();
    UpdateVisuals();
}

FReply UWindowPlacementWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && TargetWall)
    {
        FVector2D ClickPosition = InMouseEvent.GetScreenSpacePosition();
        StartPosition = GetLocalPositionOnWall(ClickPosition);
        bIsDragging = true;
        
        UpdateWindowSize(FVector2D::ZeroVector);
        
        return FReply::Handled().CaptureMouse(TakeWidget());
    }
    
    return FReply::Unhandled();
}

FReply UWindowPlacementWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsDragging)
    {
        bIsDragging = false;
        
        if (bIsValidPlacement && TargetWall && WindowComponent && CurrentSize.X > 50.0f)
        {
            WindowComponent->AddWindow(StartPosition, CurrentSize);
            RemoveFromParent();
        }
        
        return FReply::Handled().ReleaseMouseCapture();
    }
    
    return FReply::Unhandled();
}

void UWindowPlacementWidget::UpdateVisuals()
{
    if (!MainBorder)
        return;

    FLinearColor Color;
    if (!TargetWall)
    {
        Color = DefaultColor;
    }
    else
    {
        Color = bIsValidPlacement ? ValidPlacementColor : InvalidPlacementColor;
    }
    
    MainBorder->SetBrushColor(Color);
}

void UWindowPlacementWidget::FollowMouse(const FVector2D& MousePosition)
{
    if (TargetWall && GeometryComponent)
    {
        FVector2D LocalPos = GetLocalPositionOnWall(MousePosition);
        LocalPos.Y = (GeometryComponent->WallHeight - CurrentSize.Y) * 0.5f;
        
        if (bIsDragging)
        {
            LocalPos.X = FMath::Min(LocalPos.X, StartPosition.X);
        }
        
        // Convert to world position
        FVector WorldPos = GeometryComponent->GetStart() + 
            GeometryComponent->GetDirection() * LocalPos.X +
            FVector(0, 0, LocalPos.Y);
            
        // Project to screen
        FVector2D ScreenPos;
        if (GetOwningPlayer()->ProjectWorldLocationToScreen(WorldPos, ScreenPos))
        {
            FVector2D PixelPos, ViewportPos;
            USlateBlueprintLibrary::AbsoluteToViewport(GetWorld(), ScreenPos, PixelPos, ViewportPos);
            SetPositionInViewport(PixelPos, false);
        }
    }
    else
    {
        // Follow mouse in free space
        FVector2D PixelPos, ViewportPos;
        USlateBlueprintLibrary::AbsoluteToViewport(GetWorld(), MousePosition, PixelPos, ViewportPos);
        
        // Center the widget on mouse
        FVector2D WidgetSize = GetCachedGeometry().GetAbsoluteSize();
        SetPositionInViewport(PixelPos - WidgetSize * 0.5f, false);
    }
}

void UWindowPlacementWidget::UpdateWindowSize(const FVector2D& NewSize)
{
    CurrentSize = NewSize;
    
    if (MainBorder)
    {
        FWidgetTransform Transform;
        Transform.Scale = (CurrentSize.X > 0.0f) ? CurrentSize / 100.0f : InitialSize / 100.0f;
        MainBorder->SetRenderTransform(Transform);
    }
    
    ValidatePlacement();
}

void UWindowPlacementWidget::ValidatePlacement()
{
    if (!WindowComponent)
    {
        bIsValidPlacement = false;
        UpdateVisuals();
        return;
    }

    bIsValidPlacement = WindowComponent->ValidateWindowPlacement(StartPosition, CurrentSize);
    UpdateVisuals();
}

FVector2D UWindowPlacementWidget::GetLocalPositionOnWall(const FVector2D& ScreenPosition) const
{
    if (!GeometryComponent)
        return FVector2D::ZeroVector;

    FVector WorldLocation, WorldDirection;
    if (GetOwningPlayer()->DeprojectScreenPositionToWorld(
        ScreenPosition.X, ScreenPosition.Y,
        WorldLocation, WorldDirection))
    {
        FPlane WallPlane(GeometryComponent->GetDirection(), GeometryComponent->GetStart());
        FVector IntersectPoint = FMath::RayPlaneIntersection(
            WorldLocation,
            WorldLocation + WorldDirection * 10000.0f,
            WallPlane);

        return GeometryComponent->GetLocalPosition(IntersectPoint);
    }
    
    return FVector2D::ZeroVector;
}

void UWindowPlacementWidget::CleanupReferences()
{
    TargetWall = nullptr;
    GeometryComponent = nullptr;
    WindowComponent = nullptr;
    bIsValidPlacement = false;
}