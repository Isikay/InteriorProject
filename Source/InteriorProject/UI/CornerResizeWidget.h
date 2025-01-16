#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CornerResizeWidget.generated.h"

class UButton;
class AWallActor;
class UWallGeometryComponent;

UCLASS()
class INTERIORPROJECT_API UCornerResizeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void SetWallActor(AWallActor* Wall);
	void SetCornerType(bool bIsStart) { bIsStartCorner = bIsStart; }

protected:
	
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* ResizeHandle;

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FLinearColor NormalColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FLinearColor HoverColor = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FLinearColor DragColor = FLinearColor(0.0f, 0.8f, 1.0f, 1.0f);

private:
	bool bIsStartCorner;
	bool bIsDragging = false;
	FVector2D DragStartPosition;
    
	UPROPERTY()
	AWallActor* OwningWall;

	UPROPERTY()
	UWallGeometryComponent* GeometryComponent;

	FTimerHandle DragTimerHandle;
	UFUNCTION()
	void ResizeHandlePressed();
	// Ending drawing mode and starting resize mode
	UFUNCTION()
	void DrawingButtonPressed();

	
	void UpdateDragPosition();
	void UpdateWallGeometry(const FVector& NewPosition);
};