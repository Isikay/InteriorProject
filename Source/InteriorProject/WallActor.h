#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallSelectedSignature, AWallActor*, Wall);

class UWallGeometryComponent;
class UWallStateComponent;
class UWallMeshComponent;
class UWallInteractionComponent;
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
    void UpdateDrawing(const FVector& CurrentPoint);
    void EndDrawing();
    void CancelDrawing();

    // Edit Mode Handling
    UFUNCTION()
    void HandleEditModeChanged(EEditMode NewMode, EEditMode OldMode);
    void SubscribeToEditModeChanges(AIPDrawingModePawn* DrawingPawn);
    void UnsubscribeFromEditModeChanges(AIPDrawingModePawn* DrawingPawn);

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
    UWallInteractionComponent* InteractionComponent;

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

    // Event Handlers
    UFUNCTION()
    void OnWallStateChanged(EWallState NewState, EWallState OldState);
    
    UFUNCTION()
    void OnSelectionChanged(bool bSelected);
    
    UFUNCTION()
    void OnWallClicked();
    
    UFUNCTION()
    void OnWallHoverBegin();
    
    UFUNCTION()
    void OnWallHoverEnd();

    // Selection handling
    void HandleSelection();
    bool IsWallSelected() const;
    
    UFUNCTION()
    void OnWindowsModified();
    
    UFUNCTION()
    void OnGeometryChanged();
    

    // Widget Management
    void InitWidgets();
    void UpdateWidgets();
    void UpdateWidgetVisibility();
    void UpdateWidgetTransforms();
};