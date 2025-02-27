// Fill out your copyright notice in the Description page of Project Settings.


#include "GUIDetect.h"
#include "Blueprint/DragDropOperation.h"

FReply UGUIDetect::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        if(OnLeftMouseButton.IsBound())
            OnLeftMouseButton.Execute();
        return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
    }
    return FReply::Unhandled();
}

FReply UGUIDetect::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if(OnMouseMove.IsBound())
        OnMouseMove.Execute(InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()).X);
    return FReply::Unhandled();
}

void UGUIDetect::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    if(OnDragStartEnd.IsBound())
    {
        OnDragStartEnd.Execute(true);
        OutOperation = NewObject<UDragDropOperation>();
    }
}

void UGUIDetect::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if(OnDragStartEnd.IsBound())
    {
        OnDragStartEnd.Execute(false);
    }
}

void UGUIDetect::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if(OnMouseEnter.IsBound())
    {
        OnMouseEnter.Execute(InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()).X);
    }
}

void UGUIDetect::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    if(OnMouseLeave.IsBound())
    {
        OnMouseLeave.Execute();
    }
}

void UGUIDetect::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if(OnMouseEnter.IsBound())
    {
        OnMouseEnter.Execute(InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition()).X);
    }
}

void UGUIDetect::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if(OnMouseLeave.IsBound())
    {
        OnMouseLeave.Execute();
    }
}

bool UGUIDetect::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if(OnMouseMove.IsBound())
        OnMouseMove.Execute(InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition()).X);
    return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

