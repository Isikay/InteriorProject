// GUIPlaceable.cpp
#include "GUIPlaceable.h"
#include "GUIDetect.h"
#include "GUIMeasurement.h"
#include "GUIWall.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "InteriorProject/Dynamic/PlaceableActor.h"

void UGUIPlaceable::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (PlaceableSizeBox)
    {
        PlaceableSizeBox->SetWidthOverride(PlaceableWidth);
        PlaceableSizeBox->SetHeightOverride(PlaceableThickness);
    }
}

void UGUIPlaceable::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    // Spawn the 3D actor representation
    if (PlaceableClass)
    {
        PlaceableActor = GetWorld()->SpawnActor<APlaceableActor>(PlaceableClass);
        if (PlaceableActor)
        {
            PlaceableActor->SetOwnerPlaceable(this);
        }
    }
}

void UGUIPlaceable::Init(UGUIDrawingField* InDrawingField)
{
    if (!InDrawingField) return;
    
    DrawingField = InDrawingField;
    
    // Setup canvas placement
    DrawingField->GetDrawingCanvas()->AddChild(this);
    UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
    if (CanvasSlot)
    {
        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        CanvasSlot->SetAutoSize(true);
    }

   

    // Bind input handlers
    DrawingField->OnMousePositionChange.AddDynamic(this, &UGUIPlaceable::HandleMousePositionUpdate);
    DrawingField->OnRightMouseButton.AddDynamic(this, &UGUIPlaceable::DestroyPlaceable);

    // Setup wall interaction
    BindWallInteractions();

    SetSelectionState(true);
}

void UGUIPlaceable::BindWallInteractions()
{
    TArray<UWidget*> Walls = DrawingField->GetDrawingCanvas()->GetAllChildren();
    for (UWidget* Widget : Walls)
    {
        if (UGUIWall* Wall = Cast<UGUIWall>(Widget))
        {
            Wall->OnEnterLeave.BindDynamic(this, &UGUIPlaceable::HandleWallInteraction);
        }
    }
}

FReply UGUIPlaceable::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        if (bIsSelected)
        {
            return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
        }
        else
        {
            SetSelectionState(true);
            return FReply::Handled();
        }
    }
    return FReply::Unhandled();
}

void UGUIPlaceable::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    OutOperation = NewObject<UDragDropOperation>();
    OutOperation->OnDragCancelled.AddDynamic(this, &UGUIPlaceable::HandleDragCancelled);
    
    SetVisibility(ESlateVisibility::HitTestInvisible);
    DrawingField->OnLeftMouseButton.RemoveDynamic(this, &UGUIPlaceable::HandleDeselection);

    BindWallInteractions();
}

void UGUIPlaceable::HandleDragCancelled(UDragDropOperation* Operation)
{
    UE_LOG( LogTemp, Warning, TEXT("Drag Cancelled") );
    if (!bIsAttachedToWall)
    {
        AttachToWall(OwnerWall, CachedPosition);
    }
    FinalizeWallPlacement();
    DrawingField->OnLeftMouseButton.AddDynamic(this, &UGUIPlaceable::HandleDeselection);
}

void UGUIPlaceable::FinalizeWallPlacement()
{
    if (!OwnerWall || !PlaceableActor || !bCanPlace) return;

    UE_LOG( LogTemp, Warning, TEXT("Finalize Wall Placement") );

    SetVisibility(ESlateVisibility::SelfHitTestInvisible);

    UpdateMeasurementVisibility(ESlateVisibility::Hidden);
    
    // Attach to wall
    PlaceableActor->Place(OwnerWall->GetWall());
    
    if(DrawingField)
    {
        DrawingField->OnRightMouseButton.RemoveAll(this);
    }
    
    // Cleanup bindings
    CleanupWallBindings();
}

void UGUIPlaceable::CleanupWallBindings()
{
    // Unbind wall interactions
    TArray<UWidget*> Walls = DrawingField->GetDrawingCanvas()->GetAllChildren();
    for (UWidget* Widget : Walls)
    {
        if (UGUIWall* Wall = Cast<UGUIWall>(Widget))
        {
            Wall->OnEnterLeave.Unbind();
        }
    }

    // Cleanup input bindings
    if (OwnerWall)
    {
        OwnerWall->ReBindWallInteractions();
    }
}

void UGUIPlaceable::HandleMousePositionUpdate(const FVector2D& Delta)
{
    if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot))
    {
        CanvasPanelSlot->SetPosition(CanvasPanelSlot->GetPosition() + Delta);
    }
}

void UGUIPlaceable::SetPlacementValid(bool bIsValid)
{
    UE_LOG( LogTemp, Warning, TEXT("Set Placement Valid: %d"), bIsValid );
    if (bIsSelected)
    {
        bCanPlace = bIsValid;
        PlaceableImage->SetColorAndOpacity(bCanPlace ? SelectedColor : FLinearColor::Red);
        UE_LOG( LogTemp, Warning, TEXT("Can Place: %d"), bCanPlace );
    }
    else
    {
        bCanPlace = true;
    }
}

void UGUIPlaceable::UpdateWallPosition(const float& NewPosition)
{
    if (!OwnerWall || !PlaceableActor) return;

    float HalfWidth = PlaceableWidth / 2;
    float WallLength = OwnerWall->GetWallLength();
        
    // Konumu duvar sınırları içinde tut
    float ClampedPosition  = FMath::Clamp(NewPosition, HalfWidth, WallLength - HalfWidth);

    PlaceableActor->UpdatePositionY(ClampedPosition);
    if(bCanPlace)
        CachedPosition = ClampedPosition;
        
    float LeftCorner = ClampedPosition - HalfWidth;
    float RightCorner = ClampedPosition + HalfWidth;
    float LeftDistance = LeftCorner; // Duvar başlangıcına maksimum mesafe
    float RightDistance = WallLength - RightCorner; // Duvar sonuna maksimum mesafe

    for (auto Placeable : OwnerWall->GetWallOverlay()->GetAllChildren())
    {
        if (UGUIPlaceable* GUIPlaceable = Cast<UGUIPlaceable>(Placeable))
        {
            if (GUIPlaceable == this) continue; // Kendini atla
            	
            float OtherHalfWidth = GUIPlaceable->PlaceableWidth / 2;
            float OtherLeft = GUIPlaceable->CachedPosition - OtherHalfWidth;
            float OtherRight = GUIPlaceable->CachedPosition + OtherHalfWidth;

            if(OtherLeft < LeftCorner) 
            {
                float Corner = OtherLeft;
                if (OtherRight < LeftCorner)
                    Corner = OtherRight;
            		
                float Distance = LeftCorner - Corner;
                if (Distance < LeftDistance)
                {
                    LeftDistance = Distance;
                }
            }
            if( OtherRight > RightCorner) // Sağda ise
            {
                float Corner = OtherRight;
                if (OtherLeft > RightCorner)
                    Corner = OtherLeft;
					
                float Distance = Corner - RightCorner;
                if (Distance < RightDistance)
                {
                    RightDistance = Distance;
                }
            }
        }
    }
    LeftSpacer->SetSize(FVector2D(LeftCorner-LeftDistance, 1.0f));
    RightSpacer->SetSize(FVector2D(RightDistance-RightCorner, 1.0f));
    LeftMeasurement->SetMeasurement(LeftDistance);
    RightMeasurement->SetMeasurement(RightDistance);
}

void UGUIPlaceable::SetSelectionState(bool bSelect)
{
    if(bIsSelected == bSelect) return;
    
    bIsSelected = bSelect;
    
    // Update visual state
    PlaceableImage->SetColorAndOpacity(bSelect ? SelectedColor : UnselectedColor);
    PlaceableActor->SetSelected(bSelect);

    if(bSelect && OwnerWall)
        OwnerWall->SetSelectionState(false);
    
    // Update measurements visibility
    UpdateMeasurementVisibility(bSelect ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    
    // Update input bindings
    if (bSelect)
    {
        DrawingField->OnLeftMouseButton.AddDynamic(this, &UGUIPlaceable::HandleDeselection);
        UpdateWallPosition(CachedPosition);
    }
    else
    {
        DrawingField->OnLeftMouseButton.RemoveDynamic(this, &UGUIPlaceable::HandleDeselection);
    }
}

void UGUIPlaceable::UpdateMeasurementVisibility(ESlateVisibility State)
{
    RightMeasurement->SetVisibility(State);
    LeftMeasurement->SetVisibility(State);
    LeftSpacer->SetVisibility(State);
    RightSpacer->SetVisibility(State);
    OwnMeasurement->SetVisibility(State);

    if (OwnerWall)
    {
        switch (State)
        {
            case ESlateVisibility::Collapsed:
                OwnerWall->SetMeasurementsVisibility(ESlateVisibility::Visible);
                break;
            case ESlateVisibility::Hidden:
                OwnerWall->SetMeasurementsVisibility(ESlateVisibility::Visible);
                break;
            case ESlateVisibility::Visible:
                OwnerWall->SetMeasurementsVisibility(ESlateVisibility::Hidden);
                break;
            default:
                break;
        }
    }
}

void UGUIPlaceable::HandleDeselection()
{
    if (bIsSelected)
    {
        SetSelectionState(false);
    }
}

void UGUIPlaceable::HandleWallInteraction(UGUIWall* Wall, bool bIsMouseEnter, float MousePosition)
{
    if (bIsMouseEnter)
    {
        AttachToWall(Wall, MousePosition);
    }
    else
    {
        DetachFromWall();
    }
}

void UGUIPlaceable::AttachToWall(UGUIWall* Wall, float Position)
{
    if (!Wall) return;

    bIsAttachedToWall = true;
    OwnerWall = Wall;
    
    // Update visuals and position
    SetRenderTranslation(FVector2D(0.f, -20.f));
    UpdateMeasurementVisibility(ESlateVisibility::Visible);
    Wall->GetWallOverlay()->AddChild(this);
    
    // Bind wall interactions
    OwnerWall->WallImage->OnMouseMove.BindDynamic(this, &UGUIPlaceable::UpdateWallPosition);
    
    OwnerWall->WallImage->OnLeftMouseButton.BindDynamic(this, &UGUIPlaceable::FinalizeWallPlacement);
    
    // Update 3D actor
    if (PlaceableActor)
    {
        PlaceableActor->AttachToWall(Wall->GetWall(), Position);
    }

    // Update input handling
    DrawingField->OnMousePositionChange.RemoveDynamic(this, &UGUIPlaceable::HandleMousePositionUpdate);
}

void UGUIPlaceable::DetachFromWall()
{
    bIsAttachedToWall = false;
    
    if (OwnerWall)
    {
        // Cleanup wall bindings
        UpdateMeasurementVisibility(ESlateVisibility::Collapsed);
        OwnerWall->ReBindWallInteractions();
        
        if (PlaceableActor)
        {
            PlaceableActor->AttachToWall(nullptr);
        }
    }

    // Return to canvas
    DrawingField->GetDrawingCanvas()->AddChild(this);
    if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot))
    {
        CanvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        CanvasPanelSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        CanvasPanelSlot->SetAutoSize(true);
        CanvasPanelSlot->SetPosition(DrawingField->GetMousePositionOnCanvas());
    }

    // Update visuals and position
    SetRenderTranslation(FVector2D::ZeroVector);
    
    DrawingField->OnMousePositionChange.AddDynamic(this, &UGUIPlaceable::HandleMousePositionUpdate);
}

void UGUIPlaceable::DestroyPlaceable()
{
    // Cleanup wall references
    if (OwnerWall)
    {
        TArray<UWidget*> Walls = DrawingField->GetDrawingCanvas()->GetAllChildren();
        for (UWidget* Widget : Walls)
        {
            if (UGUIWall* Wall = Cast<UGUIWall>(Widget))
            {
                Wall->OnEnterLeave.Unbind();
            }
        }
        OwnerWall = nullptr;
    }

    // Cleanup 3D actor
    if (PlaceableActor)
    {
        PlaceableActor->Destroy();
        PlaceableActor = nullptr;
    }

    // Cleanup drawing field references
    if (DrawingField)
    {
        DrawingField->OnMousePositionChange.RemoveDynamic(this, &UGUIPlaceable::HandleMousePositionUpdate);
        DrawingField->OnRightMouseButton.RemoveAll(this);
        DrawingField = nullptr;
    }

    RemoveFromParent();
}
