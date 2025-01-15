#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteriorProject/Enums/InteriorTypes.h"
#include "WindowGeometryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGeometryChanged);

UCLASS()
class INTERIORPROJECT_API UWindowGeometryComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UWindowGeometryComponent();

	// Size Interface
	void SetSize(const FVector2D& NewSize);
	FVector2D GetSize() const { return WindowSize; }
	
	// Position Interface
	void SetWallSide(EWallSide Side);
	EWallSide GetWallSide() const { return CurrentWallSide; }
	void UpdatePosition(const FVector& NewPosition);

	// Geometry Properties
	UPROPERTY(EditAnywhere, Category = "Window")
	float WindowDepth = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Window")
	float MinWidth = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Window")
	float MinHeight = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Window")
	float MinWallDistance = 50.0f;

	// Events
	FOnGeometryChanged OnGeometryChanged;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	FVector2D WindowSize;

	UPROPERTY()
	EWallSide CurrentWallSide;

	void ValidateSize();
	void NotifyGeometryChanged();
};