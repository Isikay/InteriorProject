#include "DrawingToolsWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "InteriorProject/Base/IPDrawingModePawn.h"

void UDrawingToolsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Get owning pawn
    OwningPawn = Cast<AIPDrawingModePawn>(GetOwningPlayerPawn());

    // Setup button events
    if (DrawWallButton)
    {
        DrawWallButton->OnClicked.AddDynamic(this, &UDrawingToolsWidget::OnDrawWallClicked);
        DrawWallButton->OnHovered.AddDynamic(this, &UDrawingToolsWidget::OnDrawWallHovered);
        DrawWallButton->OnUnhovered.AddDynamic(this, &UDrawingToolsWidget::OnDrawWallUnhovered);
    }

    if (DrawRectangleWallButton)
    {
        DrawRectangleWallButton->OnClicked.AddDynamic(this, &UDrawingToolsWidget::OnDrawRectangleWallClicked);
        DrawRectangleWallButton->OnHovered.AddDynamic(this, &UDrawingToolsWidget::OnDrawRectangleWallHovered);
        DrawRectangleWallButton->OnUnhovered.AddDynamic(this, &UDrawingToolsWidget::OnDrawRectangleWallUnhovered);
    }

    if (AddWindowButton)
    {
        AddWindowButton->OnClicked.AddDynamic(this, &UDrawingToolsWidget::OnAddWindowClicked);
        AddWindowButton->OnHovered.AddDynamic(this, &UDrawingToolsWidget::OnAddWindowHovered);
        AddWindowButton->OnUnhovered.AddDynamic(this, &UDrawingToolsWidget::OnAddWindowUnhovered);
    }

    if (AddGateButton)
    {
        AddGateButton->OnClicked.AddDynamic(this, &UDrawingToolsWidget::OnAddGateClicked);
        AddGateButton->OnHovered.AddDynamic(this, &UDrawingToolsWidget::OnAddGateHovered);
        AddGateButton->OnUnhovered.AddDynamic(this, &UDrawingToolsWidget::OnAddGateUnhovered);
    }

    if (ToggleSnappingButton)
    {
        ToggleSnappingButton->OnClicked.AddDynamic(this, &UDrawingToolsWidget::OnToggleSnappingClicked);
        ToggleSnappingButton->OnHovered.AddDynamic(this, &UDrawingToolsWidget::OnToggleSnappingHovered);
        ToggleSnappingButton->OnUnhovered.AddDynamic(this, &UDrawingToolsWidget::OnToggleSnappingUnhovered);
    }

    // Setup icons
    SetupIcons();
}

void UDrawingToolsWidget::SetupIcons()
{
    if (DrawWallImage && DrawWallIcon)
    {
        DrawWallImage->SetBrushFromTexture(DrawWallIcon);
    }

    if (DrawRectangleWallImage && DrawRectangleWallIcon)
    {
        DrawRectangleWallImage->SetBrushFromTexture(DrawRectangleWallIcon);
    }

    if (WindowImage && WindowIcon)
    {
        WindowImage->SetBrushFromTexture(WindowIcon);
    }

    if (GateImage && GateIcon)
    {
        GateImage->SetBrushFromTexture(GateIcon);
    }
    
    if (SnappingImage && SnappingIcon)
    {
        SnappingImage->SetBrushFromTexture(SnappingIcon);
        // Set initial opacity based on snapping state
        SnappingImage->SetRenderOpacity(bSnappingEnabled ? 1.0f : 0.5f);
    }
}

void UDrawingToolsWidget::OnDrawWallClicked()
{
    if (OwningPawn)
    {
        OwningPawn->StartWallDrawing();
    }
}

void UDrawingToolsWidget::OnDrawRectangleWallClicked()
{
    if (OwningPawn)
    {
        OwningPawn->StartRectangleWallDrawing();
    }
}

void UDrawingToolsWidget::OnAddWindowClicked()
{
    if (OwningPawn)
    {
        OwningPawn->StartWindowPlacement();
    }
}

void UDrawingToolsWidget::OnAddGateClicked()
{
    if (OwningPawn)
    {
        //Not implemented yet
        //OwningPawn->StartGatePlacement();
    }
}

void UDrawingToolsWidget::OnToggleSnappingClicked()
{
    bSnappingEnabled = !bSnappingEnabled;
    
    // Update visual feedback
    if (SnappingImage)
    {
        SnappingImage->SetRenderOpacity(bSnappingEnabled ? 1.0f : 0.5f);
    }
}

void UDrawingToolsWidget::OnDrawWallHovered()
{
    UpdateButtonState(DrawWallButton, DrawWallImage, true);
}

void UDrawingToolsWidget::OnDrawWallUnhovered()
{
    UpdateButtonState(DrawWallButton, DrawWallImage, false);
}

void UDrawingToolsWidget::OnDrawRectangleWallHovered()
{
    UpdateButtonState(DrawRectangleWallButton, DrawRectangleWallImage, true);
}

void UDrawingToolsWidget::OnDrawRectangleWallUnhovered()
{
    UpdateButtonState(DrawRectangleWallButton, DrawRectangleWallImage, false);
}

void UDrawingToolsWidget::OnAddWindowHovered()
{
    UpdateButtonState(AddWindowButton, WindowImage, true);
}

void UDrawingToolsWidget::OnAddWindowUnhovered()
{
    UpdateButtonState(AddWindowButton, WindowImage, false);
}

void UDrawingToolsWidget::OnAddGateHovered()
{
    UpdateButtonState(AddGateButton, GateImage, true);
}

void UDrawingToolsWidget::OnAddGateUnhovered()
{
    UpdateButtonState(AddGateButton, GateImage, false);
}

void UDrawingToolsWidget::OnToggleSnappingHovered()
{
    UpdateButtonState(ToggleSnappingButton, SnappingImage, true);
}

void UDrawingToolsWidget::OnToggleSnappingUnhovered()
{
    UpdateButtonState(ToggleSnappingButton, SnappingImage, false);
}

void UDrawingToolsWidget::UpdateButtonState(UButton* Button, UImage* Icon, bool bIsHovered)
{
    if (!Button || !Icon)
        return;

    // Update visual feedback
    const float HoveredOpacity = 1.0f;
    const float UnhoveredOpacity = 0.7f;
    const float HoveredScale = 1.1f;
    const float UnhoveredScale = 1.0f;

    // Update opacities
    Button->SetRenderOpacity(bIsHovered ? HoveredOpacity : UnhoveredOpacity);
    Icon->SetRenderOpacity(bIsHovered ? HoveredOpacity : UnhoveredOpacity);

    // Update icon scale
    FVector2D NewScale(bIsHovered ? HoveredScale : UnhoveredScale);
    Icon->SetRenderScale(NewScale);
}
