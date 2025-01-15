#pragma once

#include "CoreMinimal.h"
#include "IPBasePawn.h"
#include "InputActionValue.h"
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

    // Mode Control
    void StartWallDrawing();
    void StartRectangleWallDrawing();
    void StartWindowPlacement();

    // Getters
    FORCEINLINE EEditMode GetCurrentEditMode() const { return CurrentEditMode; }
    FORCEINLINE AWallActor* GetSelectedWall() const { return SelectedWall; }
    FORCEINLINE bool IsDrawingWall() const { return CurrentEditMode == EEditMode::WallDrawing; }

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

    // Collision
    UPROPERTY(EditDefaultsOnly, Category = "Collision")
    TEnumAsByte<ECollisionChannel> FloorCollisionChannel;

private:
    // Input Handlers
    void Move(const FInputActionValue& Value);
    void Zoom(const FInputActionValue& Value);
    void OnLeftMousePressed();
    void OnLeftMouseReleased();
    void OnRightMousePressed();

    // Cursor Helper
    bool GetFloorLocationUnderCursor(FVector& OutLocation) const;

    // Wall Management
    UPROPERTY()
    AWallActor* CurrentWall;
    
    UPROPERTY()
    AWallActor* SelectedWall;

    void HandleWallDrawing();
    void EndWallDrawing();
    void CancelWallDrawing();
    AWallActor* SpawnWall(const FVector& Location);
    void CleanupWall();

    // Window Management
    UPROPERTY()
    AWindowActor* CurrentPlacingWindow;

    void HandleWindowPlacement();
    void EndWindowPlacement();
    void CancelWindowPlacement();
    void CleanupWindow();

    // UI
    UPROPERTY()
    UDrawingToolsWidget* DrawingToolsWidget;

    // State Management
    EEditMode CurrentEditMode;
    FVector2D DefaultWindowSize = FVector2D(100.0f, 150.0f);

    void SetEditMode(EEditMode NewMode);
    void HandleEditModeChange(EEditMode NewMode, EEditMode OldMode);

    // Wall Event Handlers
    UFUNCTION()
    void OnWallSelected(AWallActor* Wall);

    // State Update
    void UpdateCurrentAction(float DeltaTime);
};