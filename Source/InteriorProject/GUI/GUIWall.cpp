// Fill out your copyright notice in the Description page of Project Settings.


#include "GUIWall.h"
#include "GUIDetect.h"
#include "GUIDrawingField.h"
#include "GUIMeasurement.h"
#include "GUIWallHandle.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
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
    
#if WITH_EDITOR
    // Debug visualization for handle connections
    if (bShowDebugConnections)
    {
        DebugVisualizeHandleConnections();
    }
#endif
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
    else if (!bIsSelected) // Seçili değilse normal renge dön
    {
        WallImage->SetColorAndOpacity(NormalColor);
        bEndpointSnapped = false;
    }
}

bool UGUIWall::CheckForEndpointSnapping(FVector2D& Position, bool IsStartPoint)
{
    if (!bEnableSnapping || !DrawingField)
        return false;
        
    bool bDidSnap = false;
    float ClosestDistance = EndpointSnapThreshold;
    FVector2D SnapTarget = Position;
    
    // Get all walls from the canvas
    TArray<UWidget*> AllWidgets = DrawingField->GetDrawingCanvas()->GetAllChildren();
    
    for (UWidget* Widget : AllWidgets)
    {
        UGUIWall* OtherWall = Cast<UGUIWall>(Widget);
        if (!OtherWall || OtherWall == this)
            continue;
            
        // Check distance to other wall's start point
        float DistToStart = FVector2D::Distance(Position, OtherWall->StartPosition);
        if (DistToStart < ClosestDistance)
        {
            ClosestDistance = DistToStart;
            SnapTarget = OtherWall->StartPosition;
            bDidSnap = true;
        }
        
        // Check distance to other wall's end point
        float DistToEnd = FVector2D::Distance(Position, OtherWall->EndPosition);
        if (DistToEnd < ClosestDistance)
        {
            ClosestDistance = DistToEnd;
            SnapTarget = OtherWall->EndPosition;
            bDidSnap = true;
        }
        
        // Check for alignment (horizontal or vertical) with other walls' endpoints
        if (!bDidSnap)
        {
            // Horizontal alignment check
            if (FMath::Abs(Position.Y - OtherWall->StartPosition.Y) < EndpointSnapThreshold)
            {
                SnapTarget.Y = OtherWall->StartPosition.Y;
                bDidSnap = true;
            }
            else if (FMath::Abs(Position.Y - OtherWall->EndPosition.Y) < EndpointSnapThreshold)
            {
                SnapTarget.Y = OtherWall->EndPosition.Y;
                bDidSnap = true;
            }
            
            // Vertical alignment check
            if (FMath::Abs(Position.X - OtherWall->StartPosition.X) < EndpointSnapThreshold)
            {
                SnapTarget.X = OtherWall->StartPosition.X;
                bDidSnap = true;
            }
            else if (FMath::Abs(Position.X - OtherWall->EndPosition.X) < EndpointSnapThreshold)
            {
                SnapTarget.X = OtherWall->EndPosition.X;
                bDidSnap = true;
            }
        }
    }
    
    if (bDidSnap)
    {
        Position = SnapTarget;
        bEndpointSnapped = true;
        
        // Show visual feedback
        ShowSnappingFeedback(true);
        return  true;
    }
    else if (bEndpointSnapped)
    {
        // Daha önce snap olmuştu ama şimdi değil, normal renge dön
        bEndpointSnapped = false;
        ShowSnappingFeedback(false);
    }
    
    return bDidSnap;
}

bool UGUIWall::FindSnapPointNearPosition(FVector2D& Position)
{
    if (!bEnableSnapping || !DrawingField)
        return false;
        
    bool bFoundSnapPoint = false;
    float ClosestDistance = EndpointSnapThreshold;
    FVector2D SnapTarget = Position;
    
    // Get all walls from the canvas
    TArray<UWidget*> AllWidgets = DrawingField->GetDrawingCanvas()->GetAllChildren();
    
    for (UWidget* Widget : AllWidgets)
    {
        UGUIWall* OtherWall = Cast<UGUIWall>(Widget);
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
    if (!Handle)
        return;
        
    // Bağlantı eşik mesafesi (piksel cinsinden)
    const float ConnectionThreshold = EndpointSnapThreshold;
    
    // Diğer duvarların handle'larını kontrol et
    for (UWidget* Widget : AllWidgets)
    {
        UGUIWall* OtherWall = Cast<UGUIWall>(Widget);
        if (!OtherWall || OtherWall == this)
            continue;
            
        // Diğer duvarın sol handle'ına olan mesafe
        float DistToLeftHandle = FVector2D::Distance(Position, OtherWall->StartPosition);
        if (DistToLeftHandle <= ConnectionThreshold)
        {
            // Handle'ları bağla
            bool bAdded = Handle->AddHandleIsConnected(OtherWall->LeftHandle);
            if (bAdded)
            {
                OtherWall->LeftHandle->AddHandleIsConnected(Handle);
            }
        }
        
        // Diğer duvarın sağ handle'ına olan mesafe
        float DistToRightHandle = FVector2D::Distance(Position, OtherWall->EndPosition);
        if (DistToRightHandle <= ConnectionThreshold)
        {
            // Handle'ları bağla
            bool bAdded = Handle->AddHandleIsConnected(OtherWall->RightHandle);
            if (bAdded)
            {
                OtherWall->RightHandle->AddHandleIsConnected(Handle);
            }
        }
    }
}

#if WITH_EDITOR
// Handle bağlantılarını görselleştirmek için debug yardımcısı
void UGUIWall::DebugVisualizeHandleConnections()
{
    if (!GEngine || !GWorld)
        return;
        
    // Sol handle bağlantılarını debugla
    if (LeftHandle)
    {
        TArray<UGUIWallHandle*> LeftConnections = LeftHandle->GetConnectedHandles();
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, 
            FString::Printf(TEXT("Wall: %s - Left Handle: %d connections, Pos: %s"), 
            *GetName(), LeftConnections.Num(), *LeftHandle->GetPosition().ToString()));
            
        // Bağlı handle'lara dünya uzayında çizgiler çiz
        for (UGUIWallHandle* ConnHandle : LeftConnections)
        {
            if (ConnHandle)
            {
                FVector2D Start = LeftHandle->GetPosition();
                FVector2D End = ConnHandle->GetPosition();
                
                FVector WorldStart(Start.X, Start.Y, 10.0f);
                FVector WorldEnd(End.X, End.Y, 10.0f);
                
                DrawDebugLine(GWorld, WorldStart, WorldEnd, FColor::Yellow, false, 5.0f, 0, 2.0f);
            }
        }
    }
    
    // Sağ handle bağlantılarını debugla
    if (RightHandle)
    {
        TArray<UGUIWallHandle*> RightConnections = RightHandle->GetConnectedHandles();
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, 
            FString::Printf(TEXT("Wall: %s - Right Handle: %d connections, Pos: %s"), 
            *GetName(), RightConnections.Num(), *RightHandle->GetPosition().ToString()));
        
        // Bağlı handle'lara dünya uzayında çizgiler çiz
        for (UGUIWallHandle* ConnHandle : RightConnections)
        {
            if (ConnHandle)
            {
                FVector2D Start = RightHandle->GetPosition();
                FVector2D End = ConnHandle->GetPosition();
                
                FVector WorldStart(Start.X, Start.Y, 10.0f);
                FVector WorldEnd(End.X, End.Y, 10.0f);
                
                DrawDebugLine(GWorld, WorldStart, WorldEnd, FColor::Cyan, false, 5.0f, 0, 2.0f);
            }
        }
    }
}
#endif

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
}


void UGUIWall::FinishCreateWall()
{
    DrawingField->OnMousePositionChange.RemoveDynamic(this, &UGUIWall::UpdateWallEnd);
    DrawingField->OnLeftMouseButton.RemoveDynamic(this, &UGUIWall::FinishCreateWall);
    DrawingField->OnRightMouseButton.RemoveAll(this);

    // Create the wall actor
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    OwnWall = GetWorld()->SpawnActor<AWallDynamic>(WallClass,FTransform(), SpawnParameters);
    OwnWall->SetGUIWall(this);

    // Update wall position and dimensions
    UpdateWall();

    // Check for handle connections at both endpoints
    ConnectHandlesToNearbyWalls();
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
    DrawingField->OnMousePositionChange.RemoveAll(this);
}

void UGUIWall::HandleDrag(bool bIsDragStart, bool bIsLeft)
{
    if(bIsDragStart)
    {
        CreateWallVisual();
        DrawingField->OnMousePositionChange.AddDynamic(this, &UGUIWall::UpdateWallEnd);
        bIsLeftSide = bIsLeft;
        
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
