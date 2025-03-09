// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteriorProject/Enums/Enums.h"
#include "GUIDrawingField.generated.h"

class UGUISnapLine;
class UGUIDetect;
class UGUIWall;
class UGUIPlaceable;
class UGUIDrawingTools;
class UCanvasPanel;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMouseEvent, const FVector2D&, MousePosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDrawingField);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScaleChanged, FVector2D, NewScale);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDrawingToolsChanged, EDrawingTools, NewMode);

UCLASS()
class INTERIORPROJECT_API UGUIDrawingField : public UUserWidget
{
	GENERATED_BODY()

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	FVector2D CalculateMousePositionOnCanvas(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) const;

	UFUNCTION()
	void StartWallDrawing();

public:

	FORCEINLINE UCanvasPanel* GetDrawingCanvas() const { return DrawingCanvas; }

	FORCEINLINE FVector2D GetMousePositionOnCanvas() const { return MousePositionOnCanvas; }

	FORCEINLINE FVector2D GetCachedMousePosition() const { return CachedMousePosition; }
	
	void SetMode(EDrawingTools NewMode);

	FORCEINLINE EDrawingTools GetMode() const { return CurrentMode; }

	// Snap durumunu ve çizgisini güncelleme fonksiyonu
	void UpdateSnapLine(bool bShowLine, const FVector2D& StartPos, const FVector2D& EndPos);

	FORCEINLINE float GetSnapThreshold() const { return EndpointSnapThreshold; }
	
	TArray<UGUIWall*> GetAllWalls();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMouseEvent OnMousePositionChange;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FDrawingField OnLeftMouseButton;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FDrawingField OnRightMouseButton;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnScaleChanged OnScaleChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDrawingToolsChanged OnDrawingToolsChanged;
	
protected:

	//Canvas Panel
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* DrawingCanvas;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UGUISnapLine* SnapLineWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Classes|UI")
	TSubclassOf<UGUIWall> GUIWallClass;

private:

	UPROPERTY()
	FVector2D MousePositionOnCanvas;

	FVector2D CachedMousePosition;
	
	FVector2D DragMousePosition;
    
	EDrawingTools CurrentMode = EDrawingTools::None;

	bool bOwnDrag = false;

	UPROPERTY(EditAnywhere, Category = "Snapping")
	float EndpointSnapThreshold = 50.0f;  // Distance threshold for snapping

	// Snap çizgisi için gerekli değişkenler
	UPROPERTY()
	bool bShowSnapLine = false;

	UPROPERTY()
	FVector2D SnapLineStartPos;

	UPROPERTY()
	FVector2D SnapLineEndPos;
};
