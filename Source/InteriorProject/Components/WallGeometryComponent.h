#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteriorProject/Interfaces/WallGeometry.h"
#include "WallGeometryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGeometryChanged);

UCLASS()
class INTERIORPROJECT_API UWallGeometryComponent : public USceneComponent, public IWallGeometry
{
	GENERATED_BODY()

public:
	UWallGeometryComponent();

	// IWallGeometry Interface
	virtual void UpdateStartPoint(const FVector& Start) override;
	virtual void UpdateEndPoint(const FVector& End) override;
	virtual void UpdateDimensions(float Height, float Thickness) override;
	virtual FVector GetStart() const override { return WallStart; }
	virtual FVector GetEnd() const override { return WallEnd; }
	virtual float GetLength() const override;
	virtual FVector GetDirection() const override;
	virtual FVector GetCenter() const override;
	virtual FVector2D GetLocalPosition(const FVector& WorldLocation) const override;

	// Properties
	UPROPERTY(EditAnywhere, Category = "Wall Dimensions")
	float WallHeight = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Wall Dimensions")
	float WallThickness = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Wall Dimensions")
	float MinWallLength = 50.0f;

	// Events
	FOnGeometryChanged OnGeometryChanged;

private:
	FVector WallStart;
	FVector WallEnd;
	bool ValidateWallDimensions() const;
};