#pragma once

#include "CoreMinimal.h"
#include "IPBasePawn.h"
#include "InputActionValue.h"
#include "InteriorProject/Enums/InteriorTypes.h"
#include "IPDrawingModePawn.generated.h"

class IDragInterface;
class UInputAction;
class AWallActor;
class AWindowActor;
class ARoomManager;
class UDrawingToolsWidget;
class AFloorActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEditModeChanged, EEditMode, NewMode, EEditMode, OldMode);

// Delegate for handling update new position of the mouse
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMousePositionUpdate, const FVector&, NewPosition);

UCLASS()
class INTERIORPROJECT_API AIPDrawingModePawn : public AIPBasePawn
{
    GENERATED_BODY()

public:
    AIPDrawingModePawn();

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEditModeChanged OnEditModeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMousePositionUpdate OnMousePositionUpdate;

    // Drawing Control Functions
    UFUNCTION(BlueprintCallable, Category = "Drawing")
    void StartWallDrawing();
    
    UFUNCTION(BlueprintCallable, Category = "Drawing")
    void StartWindowPlacement();
    
    // Getters
    UFUNCTION(BlueprintPure, Category = "Drawing")
    FORCEINLINE EEditMode GetCurrentEditMode() const { return CurrentEditMode; }
    
    UFUNCTION(BlueprintPure, Category = "Drawing")
    FORCEINLINE UDrawingToolsWidget* GetDrawingToolsWidget() const { return DrawingToolsWidget; }
    
    FVector GetWorldPositionFromMouse() const;

    // Snapping
    FORCEINLINE void ToggleSnapping() { bSnappingEnabled = !bSnappingEnabled; }
    FORCEINLINE void SetGridSize(float NewGridSize) { GridSize = NewGridSize; }
    FORCEINLINE void SetSnapThreshold(float NewSnapThreshold) { SnapThreshold = NewSnapThreshold; }
    FORCEINLINE bool IsSnappingEnabled() const { return bSnappingEnabled; }
    FORCEINLINE float GetGridSize() const { return GridSize; }
    FORCEINLINE float GetSnapThreshold() const { return SnapThreshold; }

    

protected:
    
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // Input Actions
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* ZoomAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LeftMouseAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* RightMouseAction;

    // Camera Properties
    UPROPERTY(EditAnywhere, Category = "Camera")
    float MinZoom = 500.0f;
    
    UPROPERTY(EditAnywhere, Category = "Camera")
    float MaxZoom = 2000.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float ZoomSpeed = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float MovementSpeed = 500.0f;
    
    /** Snapping Properties */
    UPROPERTY(EditAnywhere, Category = "Snapping")
    float GridSize = 100.0f;  // Size of the snapping grid in units

    UPROPERTY(EditAnywhere, Category = "Snapping")
    float SnapThreshold = 50.0f;  // Distance within which snapping occurs

private:
    // Input Handlers
    void Move(const FInputActionValue& Value);
    void Zoom(const FInputActionValue& Value);
    
    void OnLeftMousePressed();
    void OnLeftMouseReleased();
    void OnRightMousePressed();
    void MousePositionUptade();
    bool GetActorUnderMousePosition(FVector& OutPosition) const;
    
    // State Management
    EEditMode CurrentEditMode;
    void SetEditMode(EEditMode NewMode);
    
    UPROPERTY(EditDefaultsOnly, Category = "Classes")
    TSubclassOf<AFloorActor> FloorActorClass;

    UPROPERTY(EditDefaultsOnly, Category = "Classes")
    TSubclassOf<ARoomManager> RoomManagerClass;

    // Widget Classes
    UPROPERTY(EditDefaultsOnly, Category = "Classes|UI")
    TSubclassOf<UDrawingToolsWidget> DrawingToolsWidgetClass;

    // Trace Channel For set on blueprint 
    UPROPERTY(EditDefaultsOnly, Category = "CustomCollision")
    TEnumAsByte<ETraceTypeQuery> TraceChannel;

    // UI Widget
    UPROPERTY()
    UDrawingToolsWidget* DrawingToolsWidget;
    
    // Room Manager
    UPROPERTY()
    ARoomManager* RoomManager;
    
    //Floor Actor
    UPROPERTY()
    AFloorActor* FloorActor;

    /** Snapping state */
    bool bSnappingEnabled = false;

    bool InformWallManager = false;

    FTimerHandle TimerHandle;

    FVector MousePrevLocation = FVector::ZeroVector;

    IDragInterface* DraggingObject;
};