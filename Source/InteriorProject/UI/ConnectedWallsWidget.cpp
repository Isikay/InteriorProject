#include "ConnectedWallsWidget.h"
#include "Components/Button.h"
#include "InteriorProject/WallConnection.h"


void UConnectedWallsWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UConnectedWallsWidget::SetWallConnectionActor(AWallConnection* WallConnectionPoint)
{
    OwningWallConnectionActor = WallConnectionPoint;
    if (OwningWallConnectionActor)
    {
        ConnectPointHandle->OnClicked.AddDynamic(this, &ThisClass::ResizeHandlePressed);
        ConnectPointHandle->OnHovered.AddDynamic(this, &ThisClass::ResizeHandleHovered);
        ConnectPointHandle->OnUnhovered.AddDynamic(this, &ThisClass::ResizeHandleUnhovered);
    }
}

void UConnectedWallsWidget::SetConnectedWalls(const TArray<FWallCorner>& Walls)
{
    ConnectedWalls = Walls;
}

void UConnectedWallsWidget::ResizeHandlePressed()
{
    if(OwningWallConnectionActor)
    {
        OwningWallConnectionActor->HandlePressed();
    }
}

void UConnectedWallsWidget::ResizeHandleHovered()
{
    if(OwningWallConnectionActor)
    {
        OwningWallConnectionActor->HandleHovered();
    }
}

void UConnectedWallsWidget::ResizeHandleUnhovered()
{
    if(OwningWallConnectionActor)
    {
        OwningWallConnectionActor->HandleUnhovered();
    }
}
