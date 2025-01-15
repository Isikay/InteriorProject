#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteriorProject/Enums/InteriorTypes.h"
#include "CameraControlsWidget.generated.h"

class UButton;
class UImage;

UCLASS()
class INTERIORPROJECT_API UCameraControlsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// UI Elements
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Toggle2DButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Toggle3DButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Toggle2DIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Toggle3DIcon;

	// Button Icons
	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	UTexture2D* Icon2D;

	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	UTexture2D* Icon3D;

private:
	// Button Event Handlers
	UFUNCTION()
	void OnToggle2DClicked();

	UFUNCTION()
	void OnToggle3DClicked();

	// Mode change handler
	UFUNCTION()
	void HandlePawnModeChanged(EPawnMode NewMode);
};