#include "CornerResizeWidget.h"
#include "Components/Button.h"
#include "InteriorProject/WallActor.h"
#include "InteriorProject/Components/WallGeometryComponent.h"

FReply UCornerResizeWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if(OwningWall)
    {
        OwningWall->HandleCornerClicked(bIsStartCorner);
    }
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UCornerResizeWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if(OwningWall)
    {
        OwningWall->HandleCornerHovered(bIsStartCorner, true);
    }
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UCornerResizeWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    if(OwningWall)
    {
        OwningWall->HandleCornerHovered(bIsStartCorner, false);
    }
    Super::NativeOnMouseLeave(InMouseEvent);
}

void UCornerResizeWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UCornerResizeWidget::SetWallActor(AWallActor* Wall)
{
    OwningWall = Wall;
}

void UCornerResizeWidget::ResizeHandlePressed()
{
    if(OwningWall)
    {
        OwningWall->HandleCornerClicked(bIsStartCorner);
    }
}

void UCornerResizeWidget::ResizeHandleHovered()
{
    if(OwningWall)
    {
        OwningWall->HandleCornerHovered(bIsStartCorner, true);
    }
}

void UCornerResizeWidget::ResizeHandleUnhovered()
{
    if(OwningWall)
    {
        OwningWall->HandleCornerHovered(bIsStartCorner, false);
    }
}

