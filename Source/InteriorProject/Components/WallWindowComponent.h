#pragma once

#include "CoreMinimal.h"
#include "WallComponentBase.h"
#include "WallWindowComponent.generated.h"

class UWallGeometryComponent;

DECLARE_MULTICAST_DELEGATE(FOnWindowsModified);

UCLASS()
class INTERIORPROJECT_API UWallWindowComponent : public UWallComponentBase
{
	GENERATED_BODY()

public:
	UWallWindowComponent();

	// Window Management
	bool AddWindow(const FVector2D& Position, const FVector2D& Size, bool bIsBack = false);
	void RemoveWindow(int32 WindowIndex);
	void ModifyWindow(int32 WindowIndex, const FVector2D& NewPosition, const FVector2D& NewSize);
	void ClearAllWindows();

	// Validation
	bool ValidateWindowPlacement(const FVector2D& Position, const FVector2D& Size) const;
	bool TryPlaceWindowAtLocation(const FVector& WorldLocation);

	// Getters
	const TArray<FWallWindow>& GetWindows() const { return Windows; }
	bool HasWindows() const { return Windows.Num() > 0; }

	// Events
	FOnWindowsModified OnWindowsModified;

protected:
	virtual void OnInitialize() override;
	virtual void OnCleanup() override;

private:
	UPROPERTY()
	UWallGeometryComponent* GeometryComponent;

	UPROPERTY()
	TArray<FWallWindow> Windows;

	// Window placement constraints
	UPROPERTY(EditAnywhere, Category = "Window Constraints")
	float MinWindowSpacing = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Window Constraints")
	float MinEdgeDistance = 20.0f;

	bool ValidateWindowConstraints(const FVector2D& Position, const FVector2D& Size) const;
	void NotifyWindowsModified();
};