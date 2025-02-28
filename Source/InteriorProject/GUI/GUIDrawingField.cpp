// Fill out your copyright notice in the Description page of Project Settings.

#include "GUIDrawingField.h"
#include "GUIDrawingTools.h"
#include "GUIWall.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanel.h"

FReply UGUIDrawingField::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if(OnLeftMouseButton.IsBound())
		{
			CachedMousePosition = MousePositionOnCanvas;
			MousePositionOnCanvas = CalculateMousePositionOnCanvas(InGeometry, InMouseEvent);
			OnLeftMouseButton.Broadcast();
		}
		return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	else if(InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if(OnRightMouseButton.IsBound())
		{
			CachedMousePosition = MousePositionOnCanvas;
			MousePositionOnCanvas = CalculateMousePositionOnCanvas(InGeometry, InMouseEvent);
			OnRightMouseButton.Broadcast();
			SetMode(EDrawingTools::None);
		}
		return FReply::Handled();
	}
	return FReply::Handled();
}

FReply UGUIDrawingField::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	CachedMousePosition = MousePositionOnCanvas;
	MousePositionOnCanvas = CalculateMousePositionOnCanvas(InGeometry, InMouseEvent);
	if(OnMousePositionChange.IsBound())
	{
		OnMousePositionChange.Broadcast(MousePositionOnCanvas);
	}
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UGUIDrawingField::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	MousePositionOnCanvas = FVector2D::ZeroVector;
	SetFocus();
	return;
}

FReply UGUIDrawingField::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	float WheelDelta = -InMouseEvent.GetWheelDelta() * 0.05f;
   
	// Get current transform
	FWidgetTransform Transform = DrawingCanvas->GetRenderTransform();
   
	// Calculate mouse position on canvas
	FVector2D LocalMousePosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D MousePosBeforeZoom = (LocalMousePosition - (InGeometry.GetLocalSize() * 0.5f) - Transform.Translation) / Transform.Scale;
   
	// Update scale
	FVector2D NewScale = Transform.Scale + FVector2D(WheelDelta, WheelDelta);
	NewScale = FVector2D(FMath::Clamp(NewScale.X, 0.1f, 3.0f), FMath::Clamp(NewScale.Y, 0.1f, 3.0f));
   
	// Adjust translation to keep mouse position fixed
	Transform.Translation -= MousePosBeforeZoom * (NewScale - Transform.Scale);
	Transform.Scale = NewScale;
	OnScaleChanged.Broadcast(NewScale);
	DrawingCanvas->SetRenderTransform(Transform);
   
	return FReply::Handled();
}


void UGUIDrawingField::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	SetCursor(EMouseCursor::GrabHand);
	bOwnDrag = true;
	DragMousePosition = InMouseEvent.GetScreenSpacePosition();
	OutOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UGUIWall::StaticClass());
}

void UGUIDrawingField::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	SetCursor(EMouseCursor::Default);
	bOwnDrag = false;
}

bool UGUIDrawingField::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if(bOwnDrag)
	{
		FVector2D MousePosition = InDragDropEvent.GetScreenSpacePosition();
		SetRenderTranslation(GetRenderTransform().Translation + MousePosition - DragMousePosition);
		DragMousePosition = MousePosition;
	}
	else
	{
		CachedMousePosition = MousePositionOnCanvas;
		MousePositionOnCanvas = CalculateMousePositionOnCanvas(InGeometry, InDragDropEvent);
		OnMousePositionChange.Broadcast(MousePositionOnCanvas);
	}
	return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

bool UGUIDrawingField::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	MousePositionOnCanvas = CalculateMousePositionOnCanvas(InGeometry, InDragDropEvent);
	
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UGUIDrawingField::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	MousePositionOnCanvas = CalculateMousePositionOnCanvas(InGeometry, InDragDropEvent);
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

FVector2D UGUIDrawingField::CalculateMousePositionOnCanvas(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) const
{
	// Get the absolute size of the screen/viewport
	FVector2D LocalSize = InGeometry.GetLocalSize();
        
	// Get local mouse position (0,0 at top-left, AbsoluteSize at bottom-right)
	FVector2D LocalMousePosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	
	FWidgetTransform CanvasTransform = DrawingCanvas->GetRenderTransform();
            
	// Calculate position relative to canvas center
	FVector2D CenterOffset = LocalMousePosition - (LocalSize * 0.5f);
	// Add canvas translation
	CenterOffset -= CanvasTransform.Translation;
	// Apply scale factor
	CenterOffset /= CanvasTransform.Scale;

	return CenterOffset;
}

void UGUIDrawingField::StartWallDrawing()
{
	// Spawn wall widget
	if (GUIWallClass)
	{
		UGUIWall* Wall = CreateWidget<UGUIWall>(GetWorld(), GUIWallClass);
		DrawingCanvas->AddChild(Wall);
		Wall->Init(this);
		Wall->StartCreateWall(MousePositionOnCanvas);
	}
}


void UGUIDrawingField::SetMode(EDrawingTools NewMode)
{
	if(CurrentMode == NewMode)
	{
		return;
	}

	OnRightMouseButton.Broadcast();
	
	switch (CurrentMode)
	{
		case EDrawingTools::WallDrawing:
			OnLeftMouseButton.RemoveDynamic(this, &UGUIDrawingField::StartWallDrawing);
			break;
		case EDrawingTools::Placeable:
			break;
		case EDrawingTools::WallSpliting:
			for (auto Widget : DrawingCanvas->GetAllChildren())
			{
				UGUIWall* Wall = Cast<UGUIWall>(Widget);
				if(Wall)
				{
					Wall->SetCanSplit(false);
				}
			}
			break;
		default:
			break;
	}
	CurrentMode = NewMode;
	OnDrawingToolsChanged.Broadcast(CurrentMode);
	switch (CurrentMode)
	{
		case EDrawingTools::WallDrawing:
			OnLeftMouseButton.AddDynamic(this, &UGUIDrawingField::StartWallDrawing);
			break;
		case EDrawingTools::Placeable:
			break;
		case EDrawingTools::WallSpliting:
			for (auto Widget : DrawingCanvas->GetAllChildren())
			{
				UGUIWall* Wall = Cast<UGUIWall>(Widget);
				if(Wall)
				{
					Wall->SetCanSplit(true);
				}
			}
			break;
		default:
			break;
	}
}


