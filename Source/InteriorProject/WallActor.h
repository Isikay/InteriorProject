#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteriorProject/Enums/InteriorTypes.h"
#include "WallActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallSelectedSignature, AWallActor*, Wall);

class UWallGeometryComponent;
class UWallStateComponent;
class UWallMeshComponent;
class UWallWindowComponent;
class UWidgetComponent;
class AIPDrawingModePawn;

UCLASS()
class INTERIORPROJECT_API AWallActor : public AActor
{
    GENERATED_BODY()

public:
    AWallActor();

    // Wall Events
    UPROPERTY(BlueprintAssignable, Category = "Wall Events")
    FOnWallSelectedSignature OnWallSelected;

    // Drawing Interface
    void StartDrawing(const FVector& StartPoint);
    void EndDrawing();
    void CancelDrawing();

    // Edit Mode Handling
    UFUNCTION()
    void HandleEditModeChanged(EEditMode NewMode, EEditMode OldMode);
    void SubscribeToEditModeChanges(AIPDrawingModePawn* DrawingPawn);
    void UnsubscribeFromEditModeChanges(AIPDrawingModePawn* DrawingPawn);

    void UpdateMeaseurementWidget();

    void HandleDeselection();

    // Getters
    FORCEINLINE UWallGeometryComponent* GetGeometryComponent() const { return GeometryComponent; }
    FORCEINLINE UWallStateComponent* GetStateComponent() const { return StateComponent; }
    FORCEINLINE UWallWindowComponent* GetWindowComponent() const { return WindowComponent; }

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
    UPROPERTY()
    AIPDrawingModePawn* OwningDrawingPawn;

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

    // Selection handling
    void HandleSelection();
    bool IsWallSelected() const;

    // Widget Management
    void InitWidgets();
    void UpdateWidgets();
    void UpdateWidgetTransforms();
    void UpdateWidgetVisibility();
};