#include "WallEditWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "InteriorProject/WallActor.h"
#include "InteriorProject/Components/WallGeometryComponent.h"
#include "InteriorProject/Components/WallStateComponent.h"

void UWallEditWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind button events
    if (DeleteButton)
    {
        DeleteButton->OnClicked.AddDynamic(this, &UWallEditWidget::OnDeleteButtonClicked);
    }
}

void UWallEditWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (TargetWall && IsInViewport() && ShouldBeVisible())
    {
        UpdateWidgetPosition();
        UpdateMeasurements();
    }
}

void UWallEditWidget::SetWall(AWallActor* InWall)
{
    CleanupReferences();

    TargetWall = InWall;
    if (TargetWall)
    {
        // Get component references
        GeometryComponent = TargetWall->FindComponentByClass<UWallGeometryComponent>();
        StateComponent = TargetWall->FindComponentByClass<UWallStateComponent>();

        // Setup event bindings
        if (GeometryComponent)
        {
            GeometryComponent->OnGeometryChanged.AddUObject(this, &UWallEditWidget::OnGeometryChanged);
        }

        if (StateComponent)
        {
            StateComponent->OnWallStateChanged.AddUObject(this, &UWallEditWidget::OnWallStateChanged);
        }

        // Initial updates
        UpdateWidgetPosition();
        UpdateMeasurements();
    }
}

void UWallEditWidget::OnDeleteButtonClicked()
{
    if (TargetWall)
    {
        // Cache target wall and clear references before destroying
        AWallActor* WallToDestroy = TargetWall;
        CleanupReferences();
        
        // Remove widget first
        RemoveFromParent();
        
        // Destroy the wall
        WallToDestroy->Destroy();
    }
}

void UWallEditWidget::OnGeometryChanged()
{
    if (ShouldBeVisible())
    {
        UpdateWidgetPosition();
        UpdateMeasurements();
    }
}

void UWallEditWidget::OnWallStateChanged(EWallState NewState, EWallState OldState)
{
    if (NewState == EWallState::Drawing)
    {
        RemoveFromParent();
    }
    else if (ShouldBeVisible())
    {
        UpdateWidgetPosition();
        UpdateMeasurements();
    }
}

void UWallEditWidget::UpdateWidgetPosition()
{
    if (!GeometryComponent || !GetOwningPlayer())
        return;

    // Get wall's middle point in screen space
    FVector WallCenter = GeometryComponent->GetCenter();
    FVector2D ScreenPosition;
    
    if (GetOwningPlayer()->ProjectWorldLocationToScreen(WallCenter, ScreenPosition))
    {
        // Convert to viewport position
        FVector2D ViewportPosition = ScreenPosition / UWidgetLayoutLibrary::GetViewportScale(this);
        
        // Adjust position based on widget size
        FVector2D WidgetSize = GetDesiredSize();
        ViewportPosition -= WidgetSize * 0.5f;

        // Add a slight vertical offset to not overlap with the wall
        ViewportPosition.Y -= WidgetSize.Y + 20.0f;

        // Set widget position
        SetPositionInViewport(ViewportPosition, false);
    }
}

void UWallEditWidget::UpdateMeasurements()
{
    if (!GeometryComponent)
        return;

    // Update length text
    if (LengthText)
    {
        float Length = GeometryComponent->GetLength();
        FText LengthString = FText::FromString(FString::Printf(TEXT("Length: %.1f cm"), Length));
        LengthText->SetText(LengthString);
    }

    // Update thickness text
    if (ThicknessText)
    {
        float Thickness = GeometryComponent->WallThickness;
        FText ThicknessString = FText::FromString(FString::Printf(TEXT("Thickness: %.1f cm"), Thickness));
        ThicknessText->SetText(ThicknessString);
    }

    // Update height text
    if (HeightText)
    {
        float Height = GeometryComponent->WallHeight;
        FText HeightString = FText::FromString(FString::Printf(TEXT("Height: %.1f cm"), Height));
        HeightText->SetText(HeightString);
    }
}

void UWallEditWidget::CleanupReferences()
{
    if (GeometryComponent)
    {
        GeometryComponent->OnGeometryChanged.RemoveAll(this);
        GeometryComponent = nullptr;
    }

    if (StateComponent)
    {
        StateComponent->OnWallStateChanged.RemoveAll(this);
        StateComponent = nullptr;
    }

    TargetWall = nullptr;
}

bool UWallEditWidget::ShouldBeVisible() const
{
    if (!StateComponent)
        return false;

    // Show widget when wall is selected or being edited
    return StateComponent->IsSelected() || 
           StateComponent->GetState() == EWallState::Selected;
}