#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WallEditWidget.generated.h"

class UButton;
class UTextBlock;
class AWallActor;
class UWallGeometryComponent;
class UWallStateComponent;

UCLASS()
class INTERIORPROJECT_API UWallEditWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// UI Elements
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* DeleteButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* LengthText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ThicknessText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* HeightText;

public:
	void SetWall(AWallActor* InWall);

private:
	// References
	UPROPERTY()
	AWallActor* TargetWall;

	UPROPERTY()
	UWallGeometryComponent* GeometryComponent;

	UPROPERTY()
	UWallStateComponent* StateComponent;

	// Event Handlers
	UFUNCTION()
	void OnDeleteButtonClicked();

	UFUNCTION()
	void OnGeometryChanged();

	UFUNCTION()
	void OnWallStateChanged(EWallState NewState, EWallState OldState);

	// Update Functions
	void UpdateWidgetPosition();
	void UpdateMeasurements();
	void CleanupReferences();

	// Helper Functions
	bool ShouldBeVisible() const;
};