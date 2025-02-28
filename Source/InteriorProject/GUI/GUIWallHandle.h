#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GUIWallHandle.generated.h"

class UGUIWall;
class UImage;
class UCanvasPanelSlot;

/**
 * Widget representing a connection point between walls
 * Appears at junction points and allows moving connected walls together by dragging
 */
UCLASS()
class INTERIORPROJECT_API UGUIWallHandle : public UUserWidget
{
    GENERATED_BODY()

public:
    // Initialize with owning wall
    void Init(UGUIWall* Wall);

    bool AddHandleIsConnected(UGUIWallHandle* Handle);

    void RemoveHandle(UGUIWallHandle* Handle);
    
    // Add a wall to this connection
    void AddWall(UGUIWall* Wall);
    
    // Remove a wall from this connection
    void RemoveWall(UGUIWall* Wall);

    void UpdateSelectedState(bool bIsSelected);
    
    // Show/Hide the connection point
    void SetVisible(bool bVisible);

    FORCEINLINE bool IsDragging() const { return bIsDragging; }
    
    FORCEINLINE void SetIsLeft(bool bLeft) { bIsLeft = bLeft; }
    
    // Get handle position based on whether it's left or right handle
    FORCEINLINE FVector2D GetPosition() const;
    
    // Get all connected handles
    FORCEINLINE TArray<UGUIWallHandle*> GetConnectedHandles() const { return ConnectedHandles; }

protected:
    
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    
    // Mouse interaction handlers
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

    UFUNCTION()
    void DragCancelled(UDragDropOperation* InOperation);
    
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* ConnectionImage;
    
    // Store each connected handles
    UPROPERTY()
    TArray<UGUIWallHandle*> ConnectedHandles;
    
    UPROPERTY()
    UGUIWall* OwningWall;

    UPROPERTY()
    UGUIWall* ConnectedWall;
    
    UPROPERTY(EditAnywhere, Category = "Setup")
    FLinearColor NormalColor = FLinearColor(0.2f, 0.7f, 1.0f, 1.0f);
    
    UPROPERTY(EditAnywhere, Category = "Setup")
    FLinearColor HoveredColor = FLinearColor(0.4f, 0.9f, 1.0f, 1.0f);

    UPROPERTY(EditAnywhere, Category = "Setup")
    FLinearColor ConnectedColor = FLinearColor(0.2f, 1.0f, 0.2f, 1.0f);
    
    UPROPERTY(EditAnywhere, Category = "Setup")
    float ConnectedSize = 12.0f;
    
    UPROPERTY(EditAnywhere, Category = "Setup")
    float NormalSize = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Setup")
    bool bIsLeft = true;

private:
    
    void HandleOwnDrag(const bool& bIsDragStart);
    
    // Is the connection currently being dragged
    bool bIsDragging = false;
    
    bool bIsWallSelected = false;
};