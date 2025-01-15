#include "CameraControlsWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "InteriorProject/Base/IPPlayerController.h"

void UCameraControlsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Get player controller reference for initial setup and binding
    if (AIPPlayerController* PC = Cast<AIPPlayerController>(GetOwningPlayer()))
    {
        
        // Subscribe to mode change events
        PC->OnPawnModeChanged.AddDynamic(this, &UCameraControlsWidget::HandlePawnModeChanged);

        // Setup button bindings
        if (Toggle2DButton)
        {
            // Add both widget and controller bindings
            Toggle2DButton->OnClicked.AddDynamic(this, &UCameraControlsWidget::OnToggle2DClicked);
            Toggle2DButton->OnClicked.AddDynamic(PC, &AIPPlayerController::SwitchTo2DMode);
            
        }

        if (Toggle3DButton)
        {
            // Add both widget and controller bindings
            Toggle3DButton->OnClicked.AddDynamic(this, &UCameraControlsWidget::OnToggle3DClicked);
            Toggle3DButton->OnClicked.AddDynamic(PC, &AIPPlayerController::SwitchTo3DMode);
        }

        // Update initial states based on current mode
        HandlePawnModeChanged(PC->GetCurrentPawnMode());
    }

    // Setup icons if assigned
    if (Toggle2DIcon && Icon2D)
    {
        Toggle2DIcon->SetBrushFromTexture(Icon2D);
    }

    if (Toggle3DIcon && Icon3D)
    {
        Toggle3DIcon->SetBrushFromTexture(Icon3D);
    }
}

void UCameraControlsWidget::OnToggle2DClicked()
{
    if (Toggle2DIcon)
    {
        Toggle2DIcon->SetColorAndOpacity(FLinearColor(0.8f, 0.8f, 1.0f, 1.0f));
    }
}

void UCameraControlsWidget::OnToggle3DClicked()
{
    if (Toggle3DIcon)
    {
        Toggle3DIcon->SetColorAndOpacity(FLinearColor(0.8f, 0.8f, 1.0f, 1.0f));
    }
}

void UCameraControlsWidget::HandlePawnModeChanged(EPawnMode NewMode)
{
    bool bIs2DMode = NewMode == EPawnMode::TopDown;
    
    // Update button states
    if (Toggle2DButton)
    {
        Toggle2DButton->SetIsEnabled(!bIs2DMode);
        Toggle2DButton->SetRenderOpacity(bIs2DMode ? 0.5f : 1.0f);
    }

    if (Toggle3DButton)
    {
        Toggle3DButton->SetIsEnabled(bIs2DMode);
        Toggle3DButton->SetRenderOpacity(bIs2DMode ? 1.0f : 0.5f);
    }

    // Update icon colors
    if (Toggle2DIcon)
    {
        Toggle2DIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, bIs2DMode ? 0.5f : 1.0f));
    }

    if (Toggle3DIcon)
    {
        Toggle3DIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, bIs2DMode ? 1.0f : 0.5f));
    }
}