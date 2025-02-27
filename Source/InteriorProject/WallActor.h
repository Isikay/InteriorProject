#pragma once

#include "CoreMinimal.h"
#include "FloorActor.h"
#include "GameFramework/Actor.h"
#include "Enums/InteriorTypes.h"
#include "Interfaces/DragInterface.h"
#include "UI/CornerResizeWidget.h"
#include "WallActor.generated.h"

class UWallGeometryComponent;
class UWallStateComponent;
class UWallMeshComponent;
class UWallWindowComponent;
class UWidgetComponent;
class AIPDrawingModePawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallSelectedSignature, AWallActor*, Wall);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCornerHoveredSignature, AWallActor*, Wall, bool, bIsStartCorner, bool, bIsHovered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCornerClickedSignature, AWallActor*, Wall , bool, bIsStartCorner);

UCLASS()
class INTERIORPROJECT_API AWallActor : public AActor, public IDragInterface
{
    GENERATED_BODY()

public:
    AWallActor();
    
    // Events
    UPROPERTY(BlueprintAssignable, Category = "Wall Events")
    FOnWallSelectedSignature OnWallSelected;

    UPROPERTY(BlueprintAssignable, Category = "Wall Events")
    FOnCornerHoveredSignature OnCornerHovered;

    UPROPERTY(BlueprintAssignable, Category = "Wall Events")
    FOnCornerClickedSignature OnCornerClicked;
  

    // Drawing Interface
    void StartDrawing(const FVector& StartPoint, AIPDrawingModePawn* Pawn);
    void EndDrawing();

    // IDragInterface
    virtual void Dragg(const FVector& DraggedDistance) override;
    
    // Movement Interface
    void StartMove();
    void UpdateMove(const FVector& NewLocation);
    void EndMove();
    bool IsMoving() const;
    
    // Selection Interface
    void HandleSelection();
    void HandleDeselection();
    bool IsWallSelected() const;

    // Widget Updates
    void UpdateMeasurementWidget();

    // Getters
    FORCEINLINE UWallGeometryComponent* GetGeometryComponent() const { return GeometryComponent; }
    FORCEINLINE UWallStateComponent* GetStateComponent() const { return StateComponent; }
    FORCEINLINE UWallWindowComponent* GetWindowComponent() const { return WindowComponent; }
    
    void HandleCornerHovered(bool bIsStartCorner, bool bIsHovered);
 
    void HandleCornerClicked(bool bIsStartCorner);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // Core Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWallGeometryComponent* GeometryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWallStateComponent* StateComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWallMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWallWindowComponent* WindowComponent;

    // UI Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|UI")
    UWidgetComponent* StartCornerWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|UI")
    UWidgetComponent* EndCornerWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|UI")
    UWidgetComponent* MeasurementWidget;

private:
    // Input Event Handlers
    UFUNCTION()
    void OnWallClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed);
    
    UFUNCTION()
    void OnWallHoverBegin(UPrimitiveComponent* HoveredComp);
    
    UFUNCTION()
    void OnWallHoverEnd(UPrimitiveComponent* HoveredComp);

    // State Event Handlers
    UFUNCTION()
    void OnWallStateChanged(EWallState NewState, EWallState OldState);
    
    UFUNCTION()
    void OnSelectionChanged(bool bSelected);

    UFUNCTION()
    void OnWindowsModified();
    
    UFUNCTION()
    void OnGeometryChanged();

    // Widget Management
    void InitWidgets();
    void UpdateWidgets();
    void UpdateWidgetTransforms();
    void UpdateWidgetVisibility();

    UPROPERTY()
    AIPDrawingModePawn* DrawingModePawn;

    // Movement state
    bool bIsMoving;
    FVector MoveStartLocation;
};