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

    RulerLine->SetDesiredSizeOverride(FVector2D(Length, 4.0f));
    // Set the rotation on the widget itself
    FWidgetTransform Transform;
    Transform.Angle = DegreeAngle;
    SetRenderTransform(Transform);
}