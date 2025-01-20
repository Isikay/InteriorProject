#pragma once

#include "CoreMinimal.h"
#include "IPBasePawn.h"
#include "InputActionValue.h"
#include "IPPlayerController.h"
#include "InteriorProject/Enums/InteriorTypes.h"
#include "IPDrawingModePawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEditModeChanged, EEditMode, NewMode, EEditMode, OldMode);

class UInputAction;
class AWallActor;
class AWindowActor;
class UDrawingToolsWidget;

UCLASS()
class INTERIORPROJECT_API AIPDrawingModePawn : public AIPBasePawn
{
    GENERATED_BODY()

public:
    AIPDrawingModePawn();

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEditModeChanged OnEditModeChanged;

    // Mode Control Functions
    void StartWallDrawing();
    void StartRectangleWallDrawing();
    void StartWindowPlacement();
    void EndWallDrawing();
    void EndWindowPlacement();
    void CancelWindowPlacement();

    // World Position Utils
    FVector GetWorldPositionFromMouse() const;
    FVector GetUpdatedDragPosition(bool bIsStartCorner = false) const;

    // Snapping Functions
    FVector GetSnappedLocation(const FVector& Location) const;
    TArray<AWallActor*> GetAllWalls() const;
    TArray<FVector> GetExistingWallPoints() const;
    TArray<AWallActor*> GetNearbyWalls(const FVector& Location, float Radius = 500.0f) const;

    // Getters
    FORCEINLINE EEditMode GetCurrentEditMode() const { return CurrentEditMode; }
    FORCEINLINE AWallActor* GetSelectedWall() const { return SelectedWall; }
    FORCEINLINE bool IsDrawingWall() const { return CurrentEditMode == EEditMode::WallDrawing; }
    FORCEINLINE UDrawingToolsWidget* GetDrawingToolsWidget() const { return DrawingToolsWidget; }

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

    // Actor Classes
    UPROPERTY(EditDefaultsOnly, Category = "Classes")
    TSubclassOf<AWallActor> WallActorClass;

    UPROPERTY(EditDefaultsOnly, Category = "Classes")
    TSubclassOf<AWindowActor> WindowActorClass;

    UPROPERTY(EditDefaultsOnly, Category = "Classes")
    TSubclassOf<UDrawingToolsWidget> DrawingToolsWidgetClass;

private:
    // Input Handlers
    void Move(const FInputActionValue& Value);
    void Zoom(const FInputActionValue& Value);
    void OnLeftMousePressed();
    void OnLeftMouseReleased();
    void OnRightMousePressed();

    // Wall Management
    UPROPERTY()
    AWallActor* CurrentWall;
    
    UPROPERTY()
    AWallActor* SelectedWall;
    
    void CancelWallDrawing();
    AWallActor* SpawnWall(const FVector& Location);
    void CleanupWall();

    // Window Management
    UPROPERTY()
    AWindowActor* CurrentPlacingWindow;

    void HandleWindowPlacement();
    void CleanupWindow();

    // UI
    UPROPERTY()
    UDrawingToolsWidget* DrawingToolsWidget;

    // State Management
    EEditMode CurrentEditMode;
    FVector2D DefaultWindowSize = FVector2D(100.0f, 150.0f);
    
    void HandleEditModeChange(EEditMode NewMode, EEditMode OldMode);
    void SetEditMode(EEditMode NewMode);

    // Wall Event Handlers
    UFUNCTION()
    void OnWallSelected(AWallActor* Wall);

    // State Update
    void UpdateCurrentAction(float DeltaTime);

    // Snapping Properties
    UPROPERTY(EditAnywhere, Category = "Snapping")
    float WallSnapRadius = 200.0f;

    // Helper Functions
    bool GetMouseWorldPosition(FVector& OutLocation, FVector& OutDirection) const;
    FVector SnapToNearbyWalls(const FVector& Location) const;
    bool ShouldSnapToGrid() const;
};