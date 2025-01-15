#include "WindowEditWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "InteriorProject/WindowActor.h"

void UWindowEditWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind events
    if (ResizeHandleTopLeft)
        ResizeHandleTopLeft->OnPressed.AddDynamic(this, &UWindowEditWidget::OnResizeHandlePressed);
    if (ResizeHandleTopRight)
        ResizeHandleTopRight->OnPressed.AddDynamic(this, &UWindowEditWidget::OnResizeHandlePressed);
    if (ResizeHandleBottomLeft)
        ResizeHandleBottomLeft->OnPressed.AddDynamic(this, &UWindowEditWidget::OnResizeHandlePressed);
    if (ResizeHandleBottomRight)
        ResizeHandleBottomRight->OnPressed.AddDynamic(this, &UWindowEditWidget::OnResizeHandlePressed);

    if (DeleteButton)
        DeleteButton->OnClicked.AddDynamic(this, &UWindowEditWidget::OnDeleteButtonClicked);

    bIsResizing = false;
}

void UWindowEditWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!TargetWindow || !IsInViewport())
        return;

    UpdateWidgetPosition();

    if (bIsResizing)
    {
        // Get current mouse position
        FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
        FVector2D Delta = MousePosition - ResizeStartPosition;

        // Calculate new size based on resize direction and delta
        FVector2D NewSize = InitialSize;

        // Determine which handle is being dragged and adjust size accordingly
        if (ResizeHandleTopLeft && ResizeHandleTopLeft->IsPressed())
        {
            NewSize.X -= Delta.X;
            NewSize.Y += Delta.Y;
        }
        else if (ResizeHandleTopRight && ResizeHandleTopRight->IsPressed())
        {
            NewSize.X += Delta.X;
            NewSize.Y += Delta.Y;
        }
        else if (ResizeHandleBottomLeft && ResizeHandleBottomLeft->IsPressed())
        {
            NewSize.X -= Delta.X;
            NewSize.Y -= Delta.Y;
        }
        else if (ResizeHandleBottomRight && ResizeHandleBottomRight->IsPressed())
        {
            NewSize.X += Delta.X;
            NewSize.Y -= Delta.Y;
        }

        // Update window size
        if (TargetWindow)
        {
            TargetWindow->UpdateSize(NewSize);
            UpdateSizeText();
        }
    }
}

void UWindowEditWidget::SetTargetWindow(AWindowActor* Window)
{
    TargetWindow = Window;
    if (TargetWindow)
    {
        UpdateWidgetPosition();
        UpdateSizeText();
    }
}

void UWindowEditWidget::UpdateWidgetPosition()
{
    if (!TargetWindow || !IsInViewport())
        return;

    // Get widget's desired size
    FVector2D WidgetSize = GetDesiredSize();

    // Get window's location in screen space
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
        return;

    FVector2D ScreenPosition;
    FVector WorldLocation = TargetWindow->GetActorLocation();
    
    if (PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
    {
        // Convert to viewport position
        FVector2D ViewportPosition = ScreenPosition / UWidgetLayoutLibrary::GetViewportScale(this);
        
        // Center the widget horizontally over the window
        ViewportPosition.X -= WidgetSize.X * 0.5f;

        // Position the widget above the window
        ViewportPosition.Y -= (WidgetSize.Y + 20.0f); // 20 units padding

        SetPositionInViewport(ViewportPosition, false);
    }
}

void UWindowEditWidget::SetIsValidPlacement(bool bValid)
{
    if (WindowPreview)
    {
        // Update preview color based on validity
        FLinearColor Color = bValid ? 
            FLinearColor(0.0f, 1.0f, 0.0f, 0.5f) : // Green for valid
            FLinearColor(1.0f, 0.0f, 0.0f, 0.5f);  // Red for invalid
        
        WindowPreview->SetColorAndOpacity(Color);
    }
}

void UWindowEditWidget::OnResizeHandlePressed()
{
    if (!TargetWindow)
        return;

    bIsResizing = true;
    ResizeStartPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
    InitialSize = TargetWindow->GetWindowSize();
    
    TargetWindow->StartResizing();
}

void UWindowEditWidget::OnResizeHandleReleased()
{
    if (bIsResizing && TargetWindow)
    {
        bIsResizing = false;
        TargetWindow->FinishResizing();
    }
}

void UWindowEditWidget::OnDeleteButtonClicked()
{
    if (TargetWindow)
    {
        TargetWindow->Destroy();
        TargetWindow = nullptr;
        RemoveFromParent();
    }
}

void UWindowEditWidget::UpdateSizeText()
{
    if (!SizeText || !TargetWindow)
        return;

    FVector2D Size = TargetWindow->GetWindowSize();
    FString SizeString = FString::Printf(TEXT("%.0f x %.0f"), Size.X, Size.Y);
    SizeText->SetText(FText::FromString(SizeString));
}

bool UWindowEditWidget::IsInViewport() const
{
    return GetParent() != nullptr;
}