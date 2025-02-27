#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteriorProject/Interfaces/WallGeometry.h"
#include "InteriorProject/UI/CornerResizeWidget.h"
#include "WallGeometryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGeometryChanged);

UCLASS()
class INTERIORPROJECT_API UWallGeometryComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UWallGeometryComponent();

	UFUNCTION()
	virtual void UpdateStartPoint(const FVector& Start);
	UFUNCTION()
	virtual void UpdateEndPoint(const FVector& End);
	virtual void UpdateDimensions(float Height, float Thickness);
	virtual FVector GetStart() const { return WallStart; }
	virtual FVector GetEnd() const { return WallEnd; }
	virtual FVector GetCornerLocation(bool bIsStartCorner) const;
	virtual float GetLength() const;
	virtual FVector GetDirection() const;
	virtual FVector GetCenter() const;
	virtual FVector2D GetLocalPosition(const FVector& WorldLocation) const;
	void UpdateCornerLocation(bool bIsStartCorner, const FVector& Vector);

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
};