// Fill out your copyright notice in the Description page of Project Settings.


#include "GUIWall.h"
#include "GUIDetect.h"
#include "GUIDrawingField.h"
#include "GUIMeasurement.h"
#include "GUIPlaceable.h"
#include "GUIWallHandle.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "InteriorProject/Dynamic/WallDynamic.h"

void UGUIWall::NativePreConstruct()
{
    Super::NativePreConstruct();
    
    if (MainWallSizeBox)
    {
        MainWallSizeBox->SetWidthOverride(Length);
        MainWallSizeBox->SetHeightOverride(Thickness);
    }
}

bool UGUIWall::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if(OnEnterLeave.IsBound())
        OnEnterLeave.Unbind();
    return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UGUIWall::UpdateWall()
{
    if(OwnWall)
    {
        OwnWall->UpdateDimensions(Thickness, Length, WallHeight);
        OwnWall->SetActorLocation(FVector(StartPosition.X, StartPosition.Y, 0.0f));
        OwnWall->SetActorRotation(FRotator(0.0f, Angle-90, 0.0f));
    }
}

void UGUIWall::UpdateWallPosition(const FVector2D& MousePosition)
{
    UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);

    FVector2D Delta = MousePosition - DrawingField->GetCachedMousePosition();
    StartPosition += Delta;
    EndPosition +=  Delta;
    CanvasPanelSlot->SetPosition(StartPosition);
}

void UGUIWall::Init(UGUIDrawingField* GUIDrawingField)
{
    DrawingField = GUIDrawingField;

    LeftHandle->Init(this);
    RightHandle->Init(this);
    
    UpperWallMeasurement->GetMeasurementText()->OnTextCommitted.AddDynamic(this, &UGUIWall::UpdateWallLength);
    LowerWallMeasurement->GetMeasurementText()->OnTextCommitted.AddDynamic(this, &UGUIWall::UpdateWallLength);
    WallImage->OnMouseEnter.BindDynamic(this, &UGUIWall::MouseEnter);
    WallImage->OnMouseLeave.BindDynamic(this, &UGUIWall::MouseLeave);
    WallImage->OnDragStartEnd.BindDynamic(this, &UGUIWall::WallDrag);
    WallImage->OnLeftMouseButton.BindDynamic(this, &UGUIWall::Select);

    UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
    CanvasPanelSlot->SetAlignment(FVector2D(0.0f, 0.5f));
    CanvasPanelSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
    CanvasPanelSlot->SetAutoSize(true);
    CanvasPanelSlot->SetZOrder(1);
}

void UGUIWall::StartCreateWall(FVector2D Position)
{
    // Check if we should snap to an existing endpoint
    FindSnapPointNearPosition(Position);
    
    // Set initial positions
    StartPosition = Position;
    EndPosition = Position;
    
    // Set initial canvas position
    Cast<UCanvasPanelSlot>(Slot)->SetPosition(StartPosition);
    
    // Set the snapping state based on the DrawingTools setting
    if (DrawingField)
    {
        // Find the DrawingTools widget
        TArray<UUserWidget*> AllWidgets;
        UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllWidgets, UGUIDrawingTools::StaticClass());
        
        if (AllWidgets.Num() > 0)
        {
            if (UGUIDrawingTools* DrawingTools = Cast<UGUIDrawingTools>(AllWidgets[0]))
            {
                DrawingTools->OnSnappingToggled.AddDynamic(this, &UGUIWall::SetSnapEnabled);
                bEnableSnapping = DrawingTools->IsSnappingEnabled();
            }
        }
    }
    
    // Bind mouse events for drawing
    DrawingField->OnMousePositionChange.AddDynamic(this, &UGUIWall::UpdateWallEnd);
    DrawingField->OnLeftMouseButton.AddDynamic(this, &UGUIWall::FinishCreateWall);
    DrawingField->OnRightMouseButton.AddDynamic(this, &UGUIWall::DestroyWall);
    DrawingField->OnDrawingToolsChanged.AddDynamic(this, &UGUIWall::HandleMoodChange);
}

void UGUIWall::ShowSnappingFeedback(bool bIsSnapped)
{
    if (bIsSnapped)
    {
        // Different visual feedback for endpoint snapping vs angle snapping
        if (bEndpointSnapped)
        {
            // Bright green for endpoint snapping
            WallImage->SetColorAndOpacity(FLinearColor(0.2f, 1.0f, 0.2f, 1.0f));
        }
        else
        {
            // Light green for angle snapping
            WallImage->SetColorAndOpacity(FLinearColor(0.8f, 1.0f, 0.8f, 1.0f));
        }
    }
    else
    {
        // Reset to normal color
        WallImage->SetColorAndOpacity(NormalColor);
    }
}
bool UGUIWall::CheckForEndpointSnapping(FVector2D& Position, bool IsStartPoint)
{
    if (!bEnableSnapping || !DrawingField)
        return false;

    bool bDidSnap = false;
    float ClosestDistance = DrawingField->GetSnapThreshold();
    FVector2D SnapTarget = Position;
    UGUIWallHandle* SnappedHandle = nullptr;

    // Get all walls from the canvas
    TArray<UGUIWall*> AllWalls = DrawingField->GetAllWalls();

    for (UGUIWall* OtherWall : AllWalls)
    {
        if (!OtherWall || OtherWall == this)
            continue;

        // Check distance to other wall's start point
        float DistToStart = FVector2D::Distance(Position, OtherWall->StartPosition);
        if (DistToStart < ClosestDistance)
        {
            ClosestDistance = DistToStart;
            SnapTarget = OtherWall->StartPosition;
            SnappedHandle = OtherWall->GetLeftHandle();
            bDidSnap = true;
        }

        // Check distance to other wall's end point
        float DistToEnd = FVector2D::Distance(Position, OtherWall->EndPosition);
        if (DistToEnd < ClosestDistance)
        {
            ClosestDistance = DistToEnd;
            SnapTarget = OtherWall->EndPosition;
            SnappedHandle = OtherWall->GetRightHandle();
            bDidSnap = true;
        }

        // Check for alignment (horizontal or vertical) with other walls' endpoints
        if (!bDidSnap)
        {
            // Horizontal alignment check
            if (FMath::Abs(Position.Y - OtherWall->StartPosition.Y) < DrawingField->GetSnapThreshold())
            {
                SnapTarget.Y = OtherWall->StartPosition.Y;
                bDidSnap = true;
            }
            else if (FMath::Abs(Position.Y - OtherWall->EndPosition.Y) < DrawingField->GetSnapThreshold())
            {
                SnapTarget.Y = OtherWall->EndPosition.Y;
                bDidSnap = true;
            }

            // Vertical alignment check
            if (FMath::Abs(Position.X - OtherWall->StartPosition.X) < DrawingField->GetSnapThreshold())
            {
                SnapTarget.X = OtherWall->StartPosition.X;
                bDidSnap = true;
            }
            else if (FMath::Abs(Position.X - OtherWall->EndPosition.X) < DrawingField->GetSnapThreshold())
            {
                SnapTarget.X = OtherWall->EndPosition.X;
                bDidSnap = true;
            }
        }
    }

    // Pozisyonu güncelle ve snap çizgisini göster
    if (bDidSnap)
    {
        Position = SnapTarget;
        bEndpointSnapped = true;

        // Snap çizgisini göster
        if (SnappedHandle)
        {
            FVector2D LineStart = IsStartPoint ? GetRightHandlePosition() : GetLeftHandlePosition();
            DrawingField->UpdateSnapLine(true, LineStart, SnapTarget);
        }

        // Görsel geri bildirim
        ShowSnappingFeedback(true);
        return true;
    }
    else
    {
        // Snap çizgisini gizle
        DrawingField->UpdateSnapLine(false, FVector2D::ZeroVector, FVector2D::ZeroVector);
        
        if (bEndpointSnapped)
        {
            // Daha önce snap olmuştu ama şimdi değil, normal renge dön
            bEndpointSnapped = false;
            ShowSnappingFeedback(false);
        }
    }

    return bDidSnap;
}

bool UGUIWall::FindSnapPointNearPosition(FVector2D& Position)
{
    if (!bEnableSnapping || !DrawingField)
        return false;

    bool bFoundSnapPoint = false;
    float ClosestDistance = DrawingField->GetSnapThreshold();  // EndpointSnapThreshold yerine DrawingField'dan alıyoruz
    FVector2D SnapTarget = Position;

    // Get all walls from the canvas
    TArray<UGUIWall*> AllWalls = DrawingField->GetAllWalls();

    for (UGUIWall* OtherWall : AllWalls)
    {
        if (!OtherWall || OtherWall == this)
            continue;

        // Check distance to other wall's start point
        float DistToStart = FVector2D::Distance(Position, OtherWall->StartPosition);
        if (DistToStart < ClosestDistance)
        {
            ClosestDistance = DistToStart;
            SnapTarget = OtherWall->StartPosition;
            bFoundSnapPoint = true;
        }

        // Check distance to other wall's end point
        float DistToEnd = FVector2D::Distance(Position, OtherWall->EndPosition);
        if (DistToEnd < ClosestDistance)
        {
            ClosestDistance = DistToEnd;
            SnapTarget = OtherWall->EndPosition;
            bFoundSnapPoint = true;
        }
    }

    if (bFoundSnapPoint)
    {
        Position = SnapTarget;
        ShowSnappingFeedback(true);
    }

    return bFoundSnapPoint;
}

void UGUIWall::SetSelectionState(bool bSelect)
{
    if(bIsSelected == bSelect) return;
    
    bIsSelected = bSelect;
    OwnWall->SetSelectionState(bSelect);
    WallImage->SetColorAndOpacity(bSelect ? SelectedColor : NormalColor);
    Cast<UCanvasPanelSlot>(Slot)->SetZOrder(bSelect ? 2 : 1);
    LeftHandle->UpdateSelectedState(bSelect);
    RightHandle->UpdateSelectedState(bSelect);
    if(bSelect)
    {
        DrawingField->OnLeftMouseButton.AddDynamic(this, &UGUIWall::DeSelect);
    }
    else
    {
        DrawingField->OnLeftMouseButton.RemoveAll(this);
    }
}

void UGUIWall::SetMeasurementsVisibility(ESlateVisibility Hidden)
{
    UpperWallMeasurement->SetVisibility(Hidden);
    LowerWallMeasurement->SetVisibility(Hidden);
}

void UGUIWall::ReBindWallInteractions()
{
    WallImage->OnMouseMove.Unbind();
    WallImage->OnLeftMouseButton.BindDynamic(this, &UGUIWall::Select);
}

void UGUIWall::SetSnapEnabled(bool bSnappingEnabled)
{
    bEnableSnapping = bSnappingEnabled;
    
    // If we're currently editing the wall, apply snapping immediately
    if (bIsLeftSide || bEndpointSnapped)
    {
        UpdateWallMeasurements();
    }
}

void UGUIWall::CachePosition(const float& Position)
{
    SplitPosition = Position;
}

void UGUIWall::SplitEnter(const float& Position)
{
    SplitPosition = Position;
   WallImage->OnMouseMove.BindDynamic( this, &UGUIWall::CachePosition );
}

void UGUIWall::SplitLeave()
{
    WallImage->OnMouseMove.Unbind();
}

void UGUIWall::SplitWall()
{
     // Only proceed if we have a valid wall and drawing field
    if (!OwnWall || !DrawingField)
        return;
    
    // Calculate split positions based on the cached mouse position
    FVector2D WallVector = EndPosition - StartPosition;
    float TotalLength = WallVector.Size();
    
    // Ensure the split position is within the wall length
    float SplitRatio = FMath::Clamp(SplitPosition / TotalLength, 0.1f, 0.9f);
    
    // Calculate the actual split point
    FVector2D SplitPoint = StartPosition + WallVector * SplitRatio;
    
    // Create the second wall
    UGUIWall* SecondWall = CreateWidget<UGUIWall>(GetWorld(), GetClass());
    if (!SecondWall)
        return;
    
    // Initialize the second wall
    DrawingField->GetDrawingCanvas()->AddChild(SecondWall);
    SecondWall->Init(DrawingField);
    
    // Set the properties for the second wall
    SecondWall->StartPosition = SplitPoint;
    SecondWall->EndPosition = EndPosition;
    SecondWall->Thickness = Thickness;
    SecondWall->WallHeight = WallHeight;
    SecondWall->bIsPermanentWall = true;
    
    // Update the canvas position for the second wall
    if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(SecondWall->Slot))
    {
        CanvasPanelSlot->SetAlignment(FVector2D(0.0f, 0.5f));
        CanvasPanelSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        CanvasPanelSlot->SetAutoSize(true);
        CanvasPanelSlot->SetPosition(SplitPoint);
    }
    
    // Update the second wall's visual representation
    SecondWall->UpdateWallMeasurements();
    
    // Create the 3D actor for the second wall
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AWallDynamic* SecondWallActor = GetWorld()->SpawnActor<AWallDynamic>(WallClass, FTransform(), SpawnParameters);
    
    if (SecondWallActor)
    {
        SecondWallActor->SetGUIWall(SecondWall);
        SecondWall->OwnWall = SecondWallActor;
        SecondWall->UpdateWall();
    }
    
    // Transfer any placeables on the second part of the wall
    TArray<UWidget*> PlaceableWidgets = WallOverlay->GetAllChildren();
    
    for (UWidget* Widget : PlaceableWidgets)
    {
        UGUIPlaceable* Placeable = Cast<UGUIPlaceable>(Widget);
        if (Placeable && Placeable->GetCurrentPosition() > SplitPosition)
        {
            // Move placeable to the second wall
            Placeable->DetachFromWall();
            
            // Adjust the position relative to the new wall's start point
            float RelativePosition = Placeable->GetCurrentPosition() - SplitPosition;
            
            // Attach to the new wall
            Placeable->AttachToWall(SecondWall, RelativePosition);
            Placeable->FinalizeWallPlacement();
        }
    }
    
    // Update this (first) wall's end position
    EndPosition = SplitPoint;
    UpdateWallMeasurements();
    UpdateWall();
    
    // Connect handles to maintain wall network
    SecondWall->ConnectHandlesToNearbyWalls();
    
    // Reset the selection state of both walls
    SetSelectionState(false);
    SecondWall->SetSelectionState(false);
    
    // Return to normal drawing mode
    DrawingField->SetMode(EDrawingTools::None);
}

void UGUIWall::SetCanSplit(bool Split)
{
    bCanSplit = Split;

    if(bCanSplit)
    {
        WallImage->OnMouseEnter.BindDynamic(this, &UGUIWall::SplitEnter);
        WallImage->OnMouseLeave.BindDynamic(this, &UGUIWall::SplitLeave);
        WallImage->OnLeftMouseButton.BindDynamic(this , &UGUIWall::SplitWall);
    }
    else
    {
        WallImage->OnMouseEnter.BindDynamic(this, &UGUIWall::MouseEnter);
        WallImage->OnMouseLeave.BindDynamic(this, &UGUIWall::MouseLeave);
        WallImage->OnLeftMouseButton.BindDynamic(this, &UGUIWall::Select);
    }
}

void UGUIWall::ConnectHandlesToNearbyWalls()
{
    if (!DrawingField)
        return;
    
    // Canvas'taki tüm widget'ları al
    TArray<UWidget*> AllWidgets = DrawingField->GetDrawingCanvas()->GetAllChildren();
    
    // Sol handle için bağlantı kontrolü yap
    ConnectHandleToNearbyWalls(StartPosition, LeftHandle, AllWidgets);
    
    // Sağ handle için bağlantı kontrolü yap
    ConnectHandleToNearbyWalls(EndPosition, RightHandle, AllWidgets);
}

void UGUIWall::ConnectHandleToNearbyWalls(const FVector2D& Position, UGUIWallHandle* Handle, const TArray<UWidget*>& AllWidgets)
{
    if (!Handle || !bIsPermanentWall) // Only proceed if this is a permanent wall
        return;

    // Diğer duvarların handle'larını kontrol et
    for (UWidget* Widget : AllWidgets)
    {
        UGUIWall* OtherWall = Cast<UGUIWall>(Widget);
        // Kendi duvarımızı, geçersiz duvarları veya geçici duvarları atlayalım
        if (!OtherWall || OtherWall == this || !OtherWall->bIsPermanentWall)
            continue;

        UGUIWallHandle* ClosestHandle = nullptr;
        UGUIWall* ClosestWall = nullptr;

        // Diğer duvarın sol handle'ına olan mesafe
        float DistToLeftHandle = FVector2D::Distance(Position, OtherWall->StartPosition);
        if (DistToLeftHandle < DrawingField->GetSnapThreshold())
        {
            ClosestHandle = OtherWall->LeftHandle;
            ClosestWall = OtherWall;
        }
        else
        {
            // Diğer duvarın sağ handle'ına olan mesafe
            float DistToRightHandle = FVector2D::Distance(Position, OtherWall->EndPosition);
            if (DistToRightHandle < DrawingField->GetSnapThreshold())
            {
                ClosestHandle = OtherWall->RightHandle;
                ClosestWall = OtherWall;
            }
        }

        // EndpointSnapThreshold'dan daha yakınsa, handle'ları birbirine bağla
        if (ClosestHandle && ClosestWall)
        {
            if (Handle->AddHandleIsConnected(ClosestHandle))
            {
                ClosestHandle->AddHandleIsConnected(Handle);
            }
        }
    }
}

void UGUIWall::UpdateWallEnd(const FVector2D& Position)
{
    FVector2D NewPosition = Position;
    
    if(bIsLeftSide)
    {
        // Check for endpoint snapping
        CheckForEndpointSnapping(NewPosition, true);
        
        // Update start position
        StartPosition = NewPosition;
        Cast<UCanvasPanelSlot>(Slot)->SetPosition(StartPosition);
    }
    else
    {
        // Check for endpoint snapping
        CheckForEndpointSnapping(NewPosition, false);
        
        // Update end position
        EndPosition = NewPosition;
    }
    
    UpdateWallMeasurements();
}

void UGUIWall::UpdateWallLength(const FText& Text, ETextCommit::Type CommitMethod)
{
    float NewLength = FCString::Atof(*Text.ToString())/10;
    if(NewLength > 0)
    {
        Length = NewLength;
        // Calculate the new end position
        FVector2D Delta = EndPosition - StartPosition;
        Delta.Normalize();
        EndPosition = StartPosition + Delta * Length;
        UpdateWallVisual();
    }
}

void UGUIWall::UpdateWallVisual()
{
    UpperWallMeasurement->SetMeasurement(Length);
    LowerWallMeasurement->SetMeasurement(Length);
    if(FMath::IsWithin(Angle, -90.0f, 90.0f))
    {
        LowerWallMeasurement->SetFlipText(true);
        UpperWallMeasurement->SetFlipText(false);
    }
    else
    {
        LowerWallMeasurement->SetFlipText(false);
        UpperWallMeasurement->SetFlipText(true);
    }
    MainWallSizeBox->SetWidthOverride(Length);
}

void UGUIWall::UpdateWallMeasurements()
{
    FVector2D Delta = EndPosition - StartPosition;
    Length = Delta.Size();
    
    // Get the angle between the two points
    Angle = FMath::RadiansToDegrees(FMath::Atan2(Delta.Y, Delta.X));
    
    // Apply angle snapping if enabled
    if (bEnableSnapping)
    {
        // Check if angle is close to 0, 90, 180, or 270 degrees
        float AngleMod = FMath::Abs(FMath::Fmod(Angle, 90.0f));
        
        if (AngleMod < SnapAngleThreshold || AngleMod > (90.0f - SnapAngleThreshold))
        {
            // Snap to nearest 90-degree increment
            float SnappedAngle = FMath::RoundToFloat(Angle / 90.0f) * 90.0f;
            
            // Calculate new endpoint based on snapped angle
            float Radians = FMath::DegreesToRadians(SnappedAngle);
            FVector2D Direction(FMath::Cos(Radians), FMath::Sin(Radians));
            
            // Keep the same length
            EndPosition = StartPosition + Direction * Length;
            
            // Check if the endpoint should snap to another wall's endpoint
            // If we were already snapped to an endpoint, maintain that snap
            if (bEndpointSnapped)
            {
                // Try to keep the endpoint snapped if it was already snapped
                FVector2D SnappedEndpoint = EndPosition;
                if (CheckForEndpointSnapping(SnappedEndpoint, false))
                {
                    EndPosition = SnappedEndpoint;
                    
                    // Recalculate length and angle
                    Delta = EndPosition - StartPosition;
                    Length = Delta.Size();
                    Angle = FMath::RadiansToDegrees(FMath::Atan2(Delta.Y, Delta.X));
                }
            }
            else
            {
                // Update the angle to the snapped value
                Angle = SnappedAngle;
                
                // Show visual feedback for angle snapping
                if (!bEndpointSnapped) // Endpoint snapping has priority for visual feedback
                {
                    ShowSnappingFeedback(true);
                }
            }
        }
        else if (!bEndpointSnapped) // Eğer açı snap olmadıysa ve endpoint snap da yoksa normal renge dön
        {
            ShowSnappingFeedback(false);
        }
    }
    
    SetRenderTransformAngle(Angle);
    UpdateWallVisual();
}

void UGUIWall::OnRightHandleClicked()
{
    bIsLeftSide = false;
}

void UGUIWall::OnLeftHandleClicked()
{
    bIsLeftSide = true;
}

void UGUIWall::StartUpdateWallEnd()
{
    DrawingField->OnMousePositionChange.AddDynamic(this, &UGUIWall::UpdateWallEnd);
    DrawingField->OnLeftMouseButton.AddDynamic(this, &UGUIWall::FinishUpdateWallEnd);
}

void UGUIWall::FinishUpdateWallEnd()
{
    DrawingField->OnMousePositionChange.RemoveDynamic(this, &UGUIWall::UpdateWallEnd);
    DrawingField->OnLeftMouseButton.RemoveDynamic(this, &UGUIWall::FinishUpdateWallEnd);
    DrawingField->OnRightMouseButton.RemoveAll(this);
    UpdateWall();

    // Get the affected handle based on which side was edited
    UGUIWallHandle* AffectedHandle = bIsLeftSide ? LeftHandle : RightHandle;
    FVector2D HandlePosition = bIsLeftSide ? StartPosition : EndPosition;
    
    if (DrawingField)
    {
        TArray<UWidget*> AllWidgets = DrawingField->GetDrawingCanvas()->GetAllChildren();
        ConnectHandleToNearbyWalls(HandlePosition, AffectedHandle, AllWidgets);
    }
    
    WallImage->SetColorAndOpacity(NormalColor);
}


void UGUIWall::FinishCreateWall()
{
    DrawingField->OnMousePositionChange.RemoveDynamic(this, &UGUIWall::UpdateWallEnd);
    DrawingField->OnLeftMouseButton.RemoveDynamic(this, &UGUIWall::FinishCreateWall);
    DrawingField->OnRightMouseButton.RemoveAll(this);

    // Create the wall actor - make sure this is done before connecting handles
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    OwnWall = GetWorld()->SpawnActor<AWallDynamic>(WallClass, FTransform(), SpawnParameters);
    
    if (OwnWall)
    {
        OwnWall->SetGUIWall(this);
        // Update wall position and dimensions
        UpdateWall();
    }
    
    this->bIsPermanentWall = true; // Set a custom flag

    // Now check for handle connections, only connecting to other permanent walls
    ConnectHandlesToNearbyWalls();

    WallImage->SetColorAndOpacity(NormalColor);
}

void UGUIWall::MouseEnter(const float& MousePosition)
{
    if(OnEnterLeave.IsBound())
        OnEnterLeave.Execute(this, true, MousePosition);
}

void UGUIWall::MouseLeave()
{
    if(OnEnterLeave.IsBound())
        OnEnterLeave.Execute(this, false, 0);
}

void UGUIWall::WallDrag(bool bIsStart)
{
    if(bIsStart)
    {
        CreateWallVisual();
        DrawingField->OnMousePositionChange.AddDynamic(this, &UGUIWall::UpdateWallPosition);
    }
    else
    {
        DestroyWallVisual();
    }
}

void UGUIWall::CreateWallVisual()
{
    // Bu duvarı görünmez yap
    SetVisibility(ESlateVisibility::HitTestInvisible);
    
    // Boş kontroller
    if (!DrawingField || !GetWorld())
        return;
    
    // Create Same Wall
    UGUIWall* Wall = CreateWidget<UGUIWall>(GetWorld(), GetClass());
    if (!Wall)
        return;
        
    // Wall ayarları
    Wall->WallImage->SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));
    Wall->UpperWallMeasurement->SetVisibility(ESlateVisibility::Hidden);
    Wall->LowerWallMeasurement->SetVisibility(ESlateVisibility::Hidden);
    Wall->SetVisibility(ESlateVisibility::HitTestInvisible);
    Wall->StartPosition = StartPosition;
    Wall->EndPosition = EndPosition;
    Wall->Thickness = Thickness;
    Wall->WallHeight = WallHeight;
    Wall->bIsPermanentWall = false; // Mark as temporary wall visual
    Wall->UpdateWallMeasurements();
    
    // Canvas'a ekle
    DrawingField->GetDrawingCanvas()->AddChild(Wall);
    
    // Panel ayarları
    if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Wall->Slot))
    {
        CanvasPanelSlot->SetAlignment(FVector2D(0.0f, 0.5f));
        CanvasPanelSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        CanvasPanelSlot->SetAutoSize(true);
        CanvasPanelSlot->SetPosition(StartPosition);
    }
    
    // Sürükleme bittiğinde geçici duvarı kaldır
    OnDragEnded.AddDynamic(Wall, &UGUIWall::RemoveFromParent);
}

void UGUIWall::DestroyWallVisual()
{
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    UpdateWall();
    OnDragEnded.Broadcast();
    OnDragEnded.Clear();
    
    // Get the affected handle based on which side was edited
    UGUIWallHandle* AffectedHandle = bIsLeftSide ? LeftHandle : RightHandle;
    FVector2D HandlePosition = bIsLeftSide ? StartPosition : EndPosition;
    
    if (DrawingField)
    {
        DrawingField->OnMousePositionChange.RemoveAll(this);
        TArray<UWidget*> AllWidgets = DrawingField->GetDrawingCanvas()->GetAllChildren();
        ConnectHandleToNearbyWalls(HandlePosition, AffectedHandle, AllWidgets);
    }
    
    WallImage->SetColorAndOpacity(NormalColor);
}

void UGUIWall::HandleDrag(bool bIsDragStart, bool bIsLeft)
{
    if(bIsDragStart)
    {
        bIsLeftSide = bIsLeft;
        CreateWallVisual();

        DrawingField->OnMousePositionChange.AddDynamic(this, &UGUIWall::UpdateWallEnd);
        
        // Reset snapping visual state
        bEndpointSnapped = false;
    }
    else
    {
        DestroyWallVisual();
    }
}

void UGUIWall::HandleMoodChange(EDrawingTools NewMode)
{
    switch (NewMode)
    {
        case  EDrawingTools::WallDrawing:
            SetVisibility(ESlateVisibility::HitTestInvisible);
            break;
        default:
            SetVisibility(ESlateVisibility::SelfHitTestInvisible); 
            break;
    }
}

void UGUIWall::DeSelect()
{
    SetSelectionState(false);
}

void UGUIWall::Select()
{
    SetSelectionState(true);
}

void UGUIWall::DestroyWall()
{
    if(DrawingField)
    {
        DrawingField->OnMousePositionChange.RemoveAll(this);
        DrawingField->OnLeftMouseButton.RemoveAll(this);
        DrawingField->OnRightMouseButton.RemoveAll(this);
        DrawingField->SetMode(EDrawingTools::None);
        DrawingField = nullptr;
    }
    if(OwnWall)
    {
        OwnWall->Destroy();
        OwnWall = nullptr;
    }
    RemoveFromParent();
}
