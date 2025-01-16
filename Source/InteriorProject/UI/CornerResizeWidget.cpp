#include "CornerResizeWidget.h"
#include "Components/Image.h"
#include "InteriorProject/WallActor.h"
#include "InteriorProject/Components/WallGeometryComponent.h"

void UCornerResizeWidget::NativeConstruct()
{
    Super::NativeConstruct();

    bIsDragging = false;
    
    if (ResizeHandle)
    {
        ResizeHandle->SetColorAndOpacity(NormalColor);
    }
}

void UCornerResizeWidget::SetWallActor(AWallActor* Wall)
{
    OwningWall = Wall;
    if (OwningWall)
    {
        GeometryComponent = OwningWall->FindComponentByClass<UWallGeometryComponent>();
    }
}

void UCornerResizeWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    if (ResizeHandle && !bIsDragging)
    {
        ResizeHandle->SetColorAndOpacity(HoverColor);
    }
}

void UCornerResizeWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    if (ResizeHandle && !bIsDragging)
    {
        ResizeHandle->SetColorAndOpacity(NormalColor);
    }
}

FReply UCornerResizeWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsDragging = true;
        DragStartPosition = InMouseEvent.GetScreenSpacePosition();
        
        if (ResizeHandle)
        {
            ResizeHandle->SetColorAndOpacity(DragColor);
        }

        return FReply::Handled().CaptureMouse(TakeWidget());
    }

    return FReply::Unhandled();
}

FReply UCornerResizeWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsDragging)
    {
        bIsDragging = false;
        
        if (ResizeHandle)
        {
            ResizeHandle->SetColorAndOpacity(NormalColor);
        }

        return FReply::Handled().ReleaseMouseCapture();
    }

    return FReply::Unhandled();
}

FReply UCornerResizeWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsDragging && OwningWall && GeometryComponent)
    {
        FVector WorldPos = GetWorldPositionFromMouse(InMouseEvent);
        UpdateWallGeometry(WorldPos);
        return FReply::Handled();   
    }

    return FReply::Unhandled();
}

FVector UCornerResizeWidget::GetWorldPositionFromMouse(const FPointerEvent& MouseEvent) const
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        FVector WorldLocation, WorldDirection;
        if (PC->DeprojectScreenPositionToWorld(
            MouseEvent.GetScreenSpacePosition().X,
            MouseEvent.GetScreenSpacePosition().Y,
            WorldLocation,
            WorldDirection))
        {
            // Project onto ground plane (Z = 0)
            FPlane GroundPlane(FVector::UpVector, 0);
            return FMath::RayPlaneIntersection(WorldLocation, WorldDirection, GroundPlane);
        }
    }
    return FVector::ZeroVector;
}

void UCornerResizeWidget::UpdateWallGeometry(const FVector& NewPosition)
{
    if (!GeometryComponent)
        return;

    if (bIsStartCorner)
    {
        GeometryComponent->UpdateStartPoint(NewPosition);
    }
    else
    {
        GeometryComponent->UpdateEndPoint(NewPosition);
    }
}