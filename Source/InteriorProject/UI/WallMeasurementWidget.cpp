#include "WallMeasurementWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UWallMeasurementWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Setup ruler line appearance
    if (RulerLine)
    {
        RulerLine->SetColorAndOpacity(RulerColor);
    }

    if (StartTick)
    {
        StartTick->SetColorAndOpacity(RulerColor);
    }

    if (EndTick)
    {
        EndTick->SetColorAndOpacity(RulerColor);
    }
}

void UWallMeasurementWidget::SetMeasurement(float Length)
{
    if (MeasurementText)
    {
        // Format length with appropriate units
        FText FormattedText = FText::FromString(FString::Printf(TEXT("%.0f cm"), Length));
        MeasurementText->SetText(FormattedText);
    }
}

void UWallMeasurementWidget::UpdateRulerTransform(const FVector2D& Start, const FVector2D& End)
{
    if (!RulerLine)
        return;

    // Calculate ruler properties
    FVector2D Delta = End - Start;
    float Length = Delta.Size();
    float Angle = FMath::Atan2(Delta.Y, Delta.X);
    float DegreeAngle = FMath::RadiansToDegrees(Angle);

    // Update ruler line
    UCanvasPanelSlot* LineSlot = Cast<UCanvasPanelSlot>(RulerLine->Slot);
    if (LineSlot)
    {
        LineSlot->SetPosition(Start);
        LineSlot->SetSize(FVector2D(Length, 2.0f)); // 2px thickness
        
        // Set the rotation on the widget itself
        FWidgetTransform Transform;
        Transform.Angle = DegreeAngle;
        RulerLine->SetRenderTransform(Transform);
    }

    // Update tick marks
    if (StartTick)
    {
        UCanvasPanelSlot* StartTickSlot = Cast<UCanvasPanelSlot>(StartTick->Slot);
        if (StartTickSlot)
        {
            StartTickSlot->SetPosition(Start + FVector2D(0, -5));
            
            FWidgetTransform Transform;
            Transform.Angle = DegreeAngle;
            StartTick->SetRenderTransform(Transform);
        }
    }

    if (EndTick)
    {
        UCanvasPanelSlot* EndTickSlot = Cast<UCanvasPanelSlot>(EndTick->Slot);
        if (EndTickSlot)
        {
            EndTickSlot->SetPosition(End + FVector2D(0, -5));
            
            FWidgetTransform Transform;
            Transform.Angle = DegreeAngle;
            EndTick->SetRenderTransform(Transform);
        }
    }

    // Update text position
    UpdateTextPosition(Start, End);
}

void UWallMeasurementWidget::UpdateTextPosition(const FVector2D& Start, const FVector2D& End)
{
    if (!MeasurementText)
        return;

    // Position text above the middle of the ruler
    FVector2D Center = (Start + End) * 0.5f;
    Center.Y -= TextOffset; // Offset above the ruler line

    UCanvasPanelSlot* TextSlot = Cast<UCanvasPanelSlot>(MeasurementText->Slot);
    if (TextSlot)
    {
        TextSlot->SetPosition(Center);
        TextSlot->SetAlignment(FVector2D(0.5f, 0.5f));
    }
}