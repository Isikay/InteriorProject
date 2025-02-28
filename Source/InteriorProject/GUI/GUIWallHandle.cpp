#include "GUIWallHandle.h"
#include "GUIWall.h"
#include "Components/Image.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/DragDropOperation.h"

void UGUIWallHandle::NativePreConstruct()
{
    Super::NativePreConstruct();
    
    if (ConnectionImage)
    {
        ConnectionImage->SetColorAndOpacity(NormalColor);
    }
}

void UGUIWallHandle::NativeConstruct()
{
    Super::NativeConstruct();
}

FReply UGUIWallHandle::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // DrawingField'de eventi tetikleme
        return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
    }
    // DrawingField'de eventi tetikle
    return FReply::Unhandled();
}

void UGUIWallHandle::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (ConnectionImage)
    {
        ConnectionImage->SetColorAndOpacity(HoveredColor);
    }
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UGUIWallHandle::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    if (ConnectionImage)
    {
        ConnectionImage->SetColorAndOpacity(NormalColor);
    }
    Super::NativeOnMouseLeave(InMouseEvent);
}

void UGUIWallHandle::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    // Wall is selected, disconnect from other handles
    if (bIsWallSelected)
    {
        // Disconnect from all other handles
        TArray<UGUIWallHandle*> HandlesCopy = ConnectedHandles;
        for (auto Handle : HandlesCopy)
        {
            if (Handle)
            {
                Handle->RemoveHandle(this);
            }
        }
        // Clear connected handles one time better performance
        ConnectedHandles.Empty();
        ConnectedWall = nullptr;
    }
    else
    {
        // Move all connected handles with this one
        TArray<UGUIWallHandle*> HandlesCopy = ConnectedHandles;
        for (auto Handle : HandlesCopy)
        {
            if (Handle)
            {
                Handle->HandleOwnDrag(true);
            }
        } 
    }
    
    // Start dragging this handle
    HandleOwnDrag(true);
    
    // Create the drag operation
    OutOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
    OutOperation->OnDragCancelled.AddDynamic(this, &UGUIWallHandle::DragCancelled);
}

void UGUIWallHandle::DragCancelled(UDragDropOperation* InOperation)
{
    // Stop dragging this handle
    HandleOwnDrag(false);
    
    // Ensure we stop dragging all connected handles
    for (auto Handle : ConnectedHandles)
    {
        // Only stop those we might have started dragging
        if (Handle && Handle->IsDragging())
        {
            Handle->HandleOwnDrag(false);
        }
    }
}

void UGUIWallHandle::HandleOwnDrag(const bool& bIsDragStart)
{
    if (bIsDragging == bIsDragStart) return;
    
    bIsDragging = bIsDragStart;
    
    // Tell the owning wall to start/stop dragging
    if (OwningWall)
    {
        OwningWall->HandleDrag(bIsDragging, bIsLeft);
    }
}

FVector2D UGUIWallHandle::GetPosition() const
{
    // Duvardan pozisyon bilgisini al
    if (OwningWall)
    {
        return bIsLeft ? OwningWall->GetLeftHandlePosition() : OwningWall->GetRightHandlePosition();
    }
    
    return FVector2D::ZeroVector;
}

void UGUIWallHandle::Init(UGUIWall* Wall)
{
    OwningWall = Wall;
}

bool UGUIWallHandle::AddHandleIsConnected(UGUIWallHandle* Handle)
{
    // Don't connect if handle is null, same as this one, or already connected
    if (!Handle || Handle == this || ConnectedHandles.Contains(Handle))
    {
        return false;
    }
    
    // Add to connected handles
    ConnectedHandles.Add(Handle);
    
    // Update visual feedback for connection
    if (ConnectionImage)
    {
        FVector2D Size = FVector2D(ConnectedHandles.IsEmpty() ? NormalSize : ConnectedSize);
        ConnectionImage->SetDesiredSizeOverride(Size);
    }
    
    return true;
}

void UGUIWallHandle::RemoveHandle(UGUIWallHandle* Handle)
{
    if (ConnectedHandles.Remove(Handle) > 0)
    {
        // Update visual feedback for connection
        if (ConnectionImage)
        {
            FVector2D Size = FVector2D(ConnectedHandles.IsEmpty() ? NormalSize : ConnectedSize);
            ConnectionImage->SetDesiredSizeOverride(Size);
        }
    }
}

void UGUIWallHandle::AddWall(UGUIWall* Wall)
{
    ConnectedWall = Wall;
    SetVisibility(ESlateVisibility::Collapsed);
}

void UGUIWallHandle::RemoveWall(UGUIWall* Wall)
{
    if (ConnectedWall == Wall)
    {
        ConnectedWall = nullptr;
    }
}

void UGUIWallHandle::UpdateSelectedState(bool bIsSelected)
{
    bIsWallSelected = bIsSelected;
    
    if (ConnectionImage)
    {
        ConnectionImage->SetColorAndOpacity(bIsSelected ? NormalColor : ConnectedHandles.IsEmpty() ? NormalColor : ConnectedColor);
        FVector2D Size = FVector2D(bIsWallSelected ? NormalSize : ConnectedHandles.IsEmpty() ? NormalSize : ConnectedSize);
        ConnectionImage->SetDesiredSizeOverride(Size);
    }
}

void UGUIWallHandle::SetVisible(bool bVisible)
{
    SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

