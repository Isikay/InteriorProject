#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CornerResizeWidget.generated.h"

class UImage;
class AWallActor;
class UWallGeometryComponent;

UCLASS()
class INTERIORPROJECT_API UCornerResizeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void SetWallActor(AWallActor* Wall);
	
	void SetCornerType(bool bIsStart) { bIsStartCorner = bIsStart; }

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected:
	
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ResizeHandle;

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FLinearColor NormalColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FLinearColor HoverColor = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FLinearColor DragColor = FLinearColor(0.0f, 0.8f, 1.0f, 1.0f);

private:
	bool bIsStartCorner;
    
	UPROPERTY()
	AWallActor* OwningWall;
	
	UFUNCTION()
	void ResizeHandlePressed();
	
	UFUNCTION()
	void ResizeHandleHovered();
	
	UFUNCTION()
	void ResizeHandleUnhovered();
};