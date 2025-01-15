#pragma once

#include "CoreMinimal.h"
#include "Enums/InteriorTypes.h"
#include "GameFramework/Actor.h"
#include "WindowActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UWidgetComponent;
class AWallActor;
class UWindowEditWidget;
class UWindowStateComponent;
class UWindowGeometryComponent;
class UWindowMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWindowSelectedSignature, AWindowActor*, Window);

UCLASS()
class INTERIORPROJECT_API AWindowActor : public AActor
{
    GENERATED_BODY()

public:
    AWindowActor();

    // Window Events
    UPROPERTY(BlueprintAssignable, Category = "Window Events")
    FOnWindowSelectedSignature OnWindowSelected;

    // Window Interface
    void StartPlacement(const FVector& Location);
    void UpdatePlacement(const FVector& Location);
    void FinishPlacement();
    void CancelPlacement();

    // Window Size Interface
    FVector2D GetWindowSize() const;
    void UpdateSize(const FVector2D& NewSize);
    void StartResizing();
    void FinishResizing();

    // Select Interface
    void HandleSelection();
    void HandleDeselection();
    bool IsSelected() const;
 
    // Wall Interface
    void AttachToWall(AWallActor* Wall, EWallSide Side);
    void DetachFromWall();
    bool IsAttachedToWall() const { return AttachedWall != nullptr; }
    AWallActor* GetAttachedWall() const { return AttachedWall; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // Core Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* RootSceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWindowStateComponent* StateComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWindowGeometryComponent* GeometryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWindowMeshComponent* MeshComponent;

    // UI Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* EditWidgetComponent;

    // Input Event Handlers
    UFUNCTION()
    void OnMouseClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed);

    UFUNCTION()
    void OnMouseHoverBegin(UPrimitiveComponent* HoveredComp);

    UFUNCTION()
    void OnMouseHoverEnd(UPrimitiveComponent* HoveredComp);

private:
    UPROPERTY()
    AWallActor* AttachedWall;

    UPROPERTY()
    UWindowEditWidget* EditWidget;

    void UpdateEditWidget();
    void CreateEditWidget();
    void DestroyEditWidget();
    void SetupInputBindings();
    void RemoveInputBindings();
    bool IsInteractionEnabled() const;
};