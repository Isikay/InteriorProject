#include "CornerResizeWidget.h"
#include "Components/Button.h"
#include "InteriorProject/WallActor.h"
#include "InteriorProject/Components/WallGeometryComponent.h"
#include "InteriorProject/Base/IPDrawingModePawn.h"

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

        // If it's end corner, bind to drawing functionality else bind to resize functionality directly
        if (!bIsStartCorner)
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
    if (bIsDragging)
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
            true    // looping
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

    // Get the player pawn
    if (AIPDrawingModePawn* DrawingPawn = Cast<AIPDrawingModePawn>(GetOwningPlayer()->GetPawn()))
    {
        // Get the updated position with all constraints and snapping applied
        FVector NewPosition = DrawingPawn->GetUpdatedDragPosition(bIsStartCorner);
        UpdateWallGeometry(NewPosition);
    }
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