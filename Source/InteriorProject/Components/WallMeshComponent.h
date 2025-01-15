#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "InteriorProject/Interfaces/WallVisual.h"
#include "WallMeshComponent.generated.h"

class UWallGeometryComponent;
class UWallStateComponent;
class UWallWindowComponent;

UCLASS()
class INTERIORPROJECT_API UWallMeshComponent : public UProceduralMeshComponent, public IWallVisual
{
    GENERATED_BODY()

public:
    UWallMeshComponent(const FObjectInitializer& ObjectInitializer);

    // IWallVisual Interface
    virtual void UpdateMeshGeometry() override;
    virtual void UpdateMaterials() override;
    virtual void UpdateWallMaterial(UMaterialInterface* Material) override;

    // Configuration
    void Initialize(UWallGeometryComponent* Geometry, UWallStateComponent* State, UWallWindowComponent* Windows);

    // Materials
    UPROPERTY(EditAnywhere, Category = "Materials")
    UMaterialInterface* DefaultMaterial;

    UPROPERTY(EditAnywhere, Category = "Materials")
    UMaterialInterface* HighlightMaterial;

    UPROPERTY(EditAnywhere, Category = "Materials")
    UMaterialInterface* SelectionMaterial;

    UPROPERTY(EditAnywhere, Category = "Materials")
    UMaterialInterface* DrawingMaterial;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY()
    UWallGeometryComponent* GeometryComponent;

    UPROPERTY()
    UWallStateComponent* StateComponent;

    UPROPERTY()
    UWallWindowComponent* WindowComponent;

    // Event handlers
    void OnGeometryChanged();
    void OnStateChanged(EWallState NewState, EWallState OldState);
    void OnSelectionChanged(bool bSelected);
    void OnWindowsModified();

    void CleanupComponents();
};