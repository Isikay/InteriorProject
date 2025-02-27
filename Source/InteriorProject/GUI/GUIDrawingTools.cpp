#include "GUIDrawingTools.h"
#include "GUIDrawingField.h"
#include "GUIPlaceable.h"
#include "GUIWall.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"

void UGUIDrawingTools::NativeConstruct()
{
    Super::NativeConstruct();

    // Setup button events
    if (DrawWallButton)
    {
        DrawWallButton->OnClicked.AddDynamic(this, &UGUIDrawingTools::OnDrawWallClicked);
        DrawWallButton->OnHovered.AddDynamic(this, &UGUIDrawingTools::OnDrawWallHovered);
        DrawWallButton->OnUnhovered.AddDynamic(this, &UGUIDrawingTools::OnDrawWallUnhovered);
    }

    if (DrawRectangleWallButton)
    {
        DrawRectangleWallButton->OnClicked.AddDynamic(this, &UGUIDrawingTools::OnDrawRectangleWallClicked);
        DrawRectangleWallButton->OnHovered.AddDynamic(this, &UGUIDrawingTools::OnDrawRectangleWallHovered);
        DrawRectangleWallButton->OnUnhovered.AddDynamic(this, &UGUIDrawingTools::OnDrawRectangleWallUnhovered);
    }

    if (AddWindowButton)
    {
        AddWindowButton->OnClicked.AddDynamic(this, &UGUIDrawingTools::OnAddWindowClicked);
        AddWindowButton->OnHovered.AddDynamic(this, &UGUIDrawingTools::OnAddWindowHovered);
        AddWindowButton->OnUnhovered.AddDynamic(this, &UGUIDrawingTools::OnAddWindowUnhovered);
    }

    if (AddGateButton)
    {
        AddGateButton->OnClicked.AddDynamic(this, &UGUIDrawingTools::OnAddGateClicked);
        AddGateButton->OnHovered.AddDynamic(this, &UGUIDrawingTools::OnAddGateHovered);
        AddGateButton->OnUnhovered.AddDynamic(this, &UGUIDrawingTools::OnAddGateUnhovered);
    }

    if (ToggleSnappingButton)
    {
        ToggleSnappingButton->OnClicked.AddDynamic(this, &UGUIDrawingTools::OnToggleSnappingClicked);
        ToggleSnappingButton->OnHovered.AddDynamic(this, &UGUIDrawingTools::OnToggleSnappingHovered);
        ToggleSnappingButton->OnUnhovered.AddDynamic(this, &UGUIDrawingTools::OnToggleSnappingUnhovered);
    }

    // Setup icons
    SetupIcons();
}



void UGUIDrawingTools::SetupIcons()
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
    }
}


void UGUIDrawingTools::SetDrawingField(UGUIDrawingField* InDrawingField)
{
    if(InDrawingField)
    {
        DrawingField = InDrawingField;
    }
}


void UGUIDrawingTools::OnDrawWallClicked()
{
    DrawingField->SetMode(EDrawingTools::WallDrawing);
}

void UGUIDrawingTools::OnDrawRectangleWallClicked()
{
  
}

void UGUIDrawingTools::OnAddWindowClicked()
{
    DrawingField->OnRightMouseButton.Broadcast();
    DrawingField->SetMode(EDrawingTools::Placeable);
    if (GUIWindowClass)
    {
        UGUIPlaceable* Window = CreateWidget<UGUIPlaceable>(GetWorld(), GUIWindowClass);
        Window->Init(DrawingField);
    }
}

void UGUIDrawingTools::OnAddGateClicked()
{
    DrawingField->OnRightMouseButton.Broadcast();
    DrawingField->SetMode(EDrawingTools::Placeable);
    if (GUIGateClass)
    {
        UGUIPlaceable* Gate = CreateWidget<UGUIPlaceable>(GetWorld(), GUIGateClass);
        Gate->Init(DrawingField);
    }
}

void UGUIDrawingTools::OnToggleSnappingClicked()
{
    // Toggle snapping state
    bSnappingEnabled = !bSnappingEnabled;
    
    // Update button appearance based on state
    if (SnappingImage)
    {
        // Visual feedback for active/inactive state
        SnappingImage->SetColorAndOpacity(bSnappingEnabled ? FLinearColor(0.0f, 1.0f, 0.0f, 1.0f) : FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));
    }
    
    // Update all existing walls
    TArray<UWidget*> Walls = DrawingField->GetDrawingCanvas()->GetAllChildren();
    for (UWidget* Widget : Walls)
    {
        if (UGUIWall* Wall = Cast<UGUIWall>(Widget))
        {
            Wall->SetSnapEnabled(bSnappingEnabled);
        }
    }
}

void UGUIDrawingTools::OnDrawWallHovered()
{
    UpdateButtonState(DrawWallButton, DrawWallImage, true);
}

void UGUIDrawingTools::OnDrawWallUnhovered()
{
    UpdateButtonState(DrawWallButton, DrawWallImage, false);
}

void UGUIDrawingTools::OnDrawRectangleWallHovered()
{
    UpdateButtonState(DrawRectangleWallButton, DrawRectangleWallImage, true);
}

void UGUIDrawingTools::OnDrawRectangleWallUnhovered()
{
    UpdateButtonState(DrawRectangleWallButton, DrawRectangleWallImage, false);
}

void UGUIDrawingTools::OnAddWindowHovered()
{
    UpdateButtonState(AddWindowButton, WindowImage, true);
}

void UGUIDrawingTools::OnAddWindowUnhovered()
{
    UpdateButtonState(AddWindowButton, WindowImage, false);
}

void UGUIDrawingTools::OnAddGateHovered()
{
    UpdateButtonState(AddGateButton, GateImage, true);
}

void UGUIDrawingTools::OnAddGateUnhovered()
{
    UpdateButtonState(AddGateButton, GateImage, false);
}

void UGUIDrawingTools::OnToggleSnappingHovered()
{
    UpdateButtonState(ToggleSnappingButton, SnappingImage, true);
}

void UGUIDrawingTools::OnToggleSnappingUnhovered()
{
    UpdateButtonState(ToggleSnappingButton, SnappingImage, false);
}

void UGUIDrawingTools::UpdateButtonState(UButton* Button, UImage* Icon, bool bIsHovered)
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
