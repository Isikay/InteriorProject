#pragma once
#include "CoreMinimal.h"
#include "DynamicMeshActor.h"
#include "WallDynamic.generated.h"

class UGUIWall;
class UMaterialInterface;
class UDynamicMesh;
class APlaceableActor;

UCLASS()
class INTERIORPROJECT_API AWallDynamic : public ADynamicMeshActor
{
	GENERATED_BODY()

	AWallDynamic();

public:
	
	void GenerateWall(UDynamicMesh* DynamicMesh);
	
	void AddHoleGeometry(APlaceableActor* PlaceableActor);
	
	void UpdateDimensions(float NewThickness, float NewWidth, float NewHeight);

	FORCEINLINE float GetWallHeight() const { return WallHeight; }
	FORCEINLINE float GetWallWidth() const { return WallWidth; }
	
	void RemoveHole(APlaceableActor* PlaceableDynamic);

	void UpdateWall();
	
	FORCEINLINE void AddPlaceable(APlaceableActor* Placeable) { Placeables.Add(Placeable); }
	FORCEINLINE void RemovePlaceable(APlaceableActor* Placeable) { Placeables.Remove(Placeable); }
	
	FORCEINLINE void SetGUIWall(UGUIWall* Wall) { GUIWall = Wall; }
	FORCENOINLINE UGUIWall* GetGUIWall() const { return GUIWall; }

	void SetSelectionState(bool bSelected);

protected:
	
	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void ClickedComponent(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	float WallThickness = 20.0f;

	float WallHeight = 300.0f;

	float WallWidth = 500.0f;

	float Angle = 0.0f;

	UPROPERTY(VisibleAnywhere)
	TArray<APlaceableActor*> Placeables;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* SelectedMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* NormalMaterial;

	UPROPERTY()
	UGUIWall* GUIWall;
};