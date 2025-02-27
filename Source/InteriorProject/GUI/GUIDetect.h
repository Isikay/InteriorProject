// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GUIDetect.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FMousePosition, const float&, Position);
DECLARE_DYNAMIC_DELEGATE(FOnMouseEvents);
DECLARE_DYNAMIC_DELEGATE_OneParam(FStartDragging, bool, bIsStart);


/**
 * 
 */
UCLASS()
class INTERIORPROJECT_API UGUIDetect : public UUserWidget
{
	GENERATED_BODY()
	
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;


protected:
	
	/*
	UPROPERTY(EditAnywhere, Category=Visual)
	FLinearColor PressedColor;

	UPROPERTY(EditAnywhere, Category=Visual)
	FLinearColor DisabledColor;

	UPROPERTY(EditAnywhere, Category=Visual)
	FLinearColor DraggingColor;*/

public:
	
	FMousePosition OnMouseEnter;

	FMousePosition OnMouseMove;

	FStartDragging OnDragStartEnd;
	
	FOnMouseEvents OnLeftMouseButton;

	FOnMouseEvents OnMouseLeave;
};
