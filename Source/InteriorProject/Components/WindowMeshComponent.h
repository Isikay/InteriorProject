#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "WindowMeshComponent.generated.h"

class UWindowGeometryComponent;
class UWindowStateComponent;

UCLASS()
class INTERIORPROJECT_API UWindowMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UWindowMeshComponent();

	// Initialization
	void Initialize(UWindowGeometryComponent* Geometry, UWindowStateComponent* State);

	// Materials
	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* DefaultMaterial;

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* PlacementMaterial;

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* InvalidPlacementMaterial;

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* SelectedMaterial;

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* HighlightMaterial;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	UWindowGeometryComponent* GeometryComponent;

	UPROPERTY()
	UWindowStateComponent* StateComponent;

	// Mesh properties
	FVector OriginalMeshBounds;

	// Event handlers
	void OnGeometryChanged();
	void OnStateChanged(EWindowEditState NewState, EWindowEditState OldState);
	void OnSelectionChanged(bool bSelected);

	// Update functions
	void UpdateMeshScale();
	void UpdateMaterial();
	void CleanupComponents();
};