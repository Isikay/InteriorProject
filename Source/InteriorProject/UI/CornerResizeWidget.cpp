#include "CornerResizeWidget.h"
#include "Components/Button.h"
#include "InteriorProject/WallActor.h"
#include "InteriorProject/Components/WallGeometryComponent.h"

void UCornerResizeWidget::NativeConstruct()
{
    Super::NativeConstruct();
}


void UCornerResizeWidget::SetWallActor(AWallActor* Wall)
{
    OwningWall = Wall;
    if (OwningWall)
    {
        GeometryComponent = OwningWall->FindComponentByClass<UWallGeometryComponent>();

        // If its end corner, bind to drawing functionaltiy else bind to resize functionality directly
        if(!bIsStartCorner)
        {
            ResizeHandlePressed();
            ResizeHandle->OnClicked.AddDynamic(this, &ThisClass::DrawingButtonPressed);
        }
        else
        {
            ResizeHandle->OnClicked.AddDynamic(this, &ThisClass::ResizeHandlePressed);
        }
    }
}

void UCornerResizeWidget::ResizeHandlePressed()
{
    if(bIsDragging)
    {
        // Clear the timer when drag ends
        GetWorld()->GetTimerManager().ClearTimer(DragTimerHandle);
        bIsDragging = false;
    }
    else
    {
        bIsDragging = true;
        // Start the timer when drag begins
        GetWorld()->GetTimerManager().SetTimer(
            DragTimerHandle,
            this,
            &UCornerResizeWidget::UpdateDragPosition,
            0.016f, // 60fps
            true // looping
            );
    }
    
}

void UCornerResizeWidget::DrawingButtonPressed()
{
    ResizeHandle->OnClicked.Clear();
    ResizeHandlePressed();
    ResizeHandle->OnClicked.AddDynamic(this, &ThisClass::ResizeHandlePressed);
    OwningWall->EndDrawing();
}


void UCornerResizeWidget::UpdateDragPosition()
{
    if (!OwningWall || !GeometryComponent)
        return;
    
    FVector WorldLocation,WorldDirection;
    GetOwningPlayer()->DeprojectMousePositionToWorld(WorldLocation,WorldDirection);
    WorldLocation.Z = 0;
    UpdateWallGeometry(WorldLocation);
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
