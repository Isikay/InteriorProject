// GUIPlaceable.h
#pragma once

#include "CoreMinimal.h"
#include "GUIDrawingField.h"
#include "Blueprint/UserWidget.h"
#include "GUIPlaceable.generated.h"

class UImage;
class USpacer;
class APlaceableActor;
class UGUIWall;
class UGUIMeasurement;
class USizeBox;

/**
 * Widget for handling placeable objects (windows, doors) in the house builder.
 * Manages drag-drop placement, wall attachment, and positioning of architectural elements.
 */
UCLASS()
class INTERIORPROJECT_API UGUIPlaceable : public UUserWidget
{
    GENERATED_BODY()

private:
    /** Reference to the wall this placeable is attached to */
    UPROPERTY()
    UGUIWall* OwnerWall;

    /** Reference to the main drawing field */
    UPROPERTY()
    UGUIDrawingField* DrawingField;

    /** Main widget lifecycle methods */
    virtual void NativePreConstruct() override;
    virtual void NativeOnInitialized() override;
    virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

protected:
    /** UI Components */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    USizeBox* PlaceableSizeBox;

    /** Measurement widgets for showing distances */
    UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
    UGUIMeasurement* OwnMeasurement;

    UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
    UGUIMeasurement* LeftMeasurement;

    UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
    UGUIMeasurement* RightMeasurement;

    /** Spacer components for visual layout */
    UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
    USpacer* LeftSpacer;

    UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
    USpacer* RightSpacer;

    /** Main visual representation */
    UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
    UImage* PlaceableImage;

    /** Visual state colors */
    UPROPERTY(EditAnywhere, Category = "Appearance")
    FLinearColor UnselectedColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, Category = "Appearance")
    FLinearColor SelectedColor = FLinearColor::Green;

    /** 3D actor reference */
    UPROPERTY()
    APlaceableActor* PlaceableActor;

    /** Actor class to spawn */
    UPROPERTY(EditAnywhere, Category = "Setup")
    TSubclassOf<APlaceableActor> PlaceableClass;

    /** Placeable dimensions */
    UPROPERTY(EditAnywhere, Category = "Dimensions")
    float PlaceableWidth = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Dimensions")
    float PlaceableHeight = 200.0f;

    UPROPERTY(EditAnywhere, Category = "Dimensions")
    float PlaceableThickness = 20.0f;

private:
    /** Current position along the wall */
    float CachedPosition;

    /** State flags */
    bool bIsSelected = false;
    bool bCanPlace = true;
    bool bIsAttachedToWall = false;

    /** Drag-drop handlers */
    UFUNCTION()
    void HandleDragCancelled(UDragDropOperation* Operation);

    UFUNCTION()
    void UpdatePlaceablePosition(const FVector2D& Delta);

    UFUNCTION()
    void HandleDeselection();
    
    void UpdateMeasurementVisibility(ESlateVisibility State);

    /** Wall interaction handlers */
    UFUNCTION()
    void HandleWallInteraction(UGUIWall* Wall, bool bIsMouseEnter, float MousePosition);
  

    /** Initialization helpers */
    void BindWallInteractions();

    /** Cleanup helpers */
    void CleanupWallBindings();
    
    /** Cleanup and destroy the placeable */
    UFUNCTION()
    void DestroyPlaceable();

public:
    /** Initialize the placeable with required references */
    void Init(UGUIDrawingField* DrawingField);

    /** Update placement validity state */
    void SetPlacementValid(bool bIsValid);
    
    UFUNCTION()
    void FinalizeWallPlacement();
    
    /** Get the current position on wall */
    FORCEINLINE float GetCurrentPosition() const { return CachedPosition; }

    /** Get the placeable width */
    FORCEINLINE float GetPlaceableWidth() const { return PlaceableWidth; }

    /** Check if placeable can be placed at current position */
    FORCEINLINE bool CanPlaceAtCurrentPosition() const { return bCanPlace; }

    /** Selection management */
    void SetSelectionState(bool bSelect);
    FORCEINLINE bool GetSelectedState() const { return bIsSelected; }

    /** Attachment Selection */
    FORCEINLINE bool IsAttachedToWall() const { return bIsAttachedToWall; }
    void AttachToWall(UGUIWall* Wall, float Position = 0.0f);
    void DetachFromWall();

    /** Position and measurement updates */
    UFUNCTION()
    void UpdateWallPosition(const float& NewPosition);
};