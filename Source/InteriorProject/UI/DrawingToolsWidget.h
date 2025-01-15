#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DrawingToolsWidget.generated.h"

class UButton;
class UImage;
class AIPDrawingModePawn;

/**
 * Widget that provides drawing tools for placing walls, windows, and gates in top-down mode
 */
UCLASS()
class INTERIORPROJECT_API UDrawingToolsWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    /** UI Elements - Only creation tools */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* DrawWallButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* DrawRectangleWallButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* AddWindowButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* AddGateButton;

    /** Button Icons */
    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    UTexture2D* DrawWallIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    UTexture2D* DrawRectangleWallIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    UTexture2D* WindowIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    UTexture2D* GateIcon;

    /** Icon Images */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* DrawWallImage;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* DrawRectangleWallImage;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* WindowImage;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* GateImage;

private:
    /** Button Event Handlers */
    UFUNCTION()
    void OnDrawWallClicked();

    UFUNCTION()
    void OnDrawRectangleWallClicked();

    UFUNCTION()
    void OnAddWindowClicked();

    UFUNCTION()
    void OnAddGateClicked();

    /** Button Hover Events */
    UFUNCTION()
    void OnDrawWallHovered();

    UFUNCTION()
    void OnDrawWallUnhovered();

    UFUNCTION()
    void OnDrawRectangleWallHovered();

    UFUNCTION()
    void OnDrawRectangleWallUnhovered();

    UFUNCTION()
    void OnAddWindowHovered();

    UFUNCTION()
    void OnAddWindowUnhovered();

    UFUNCTION()
    void OnAddGateHovered();

    UFUNCTION()
    void OnAddGateUnhovered();

    /** The owning pawn reference */
    UPROPERTY()
    AIPDrawingModePawn* OwningPawn;

    /** Visual feedback helpers */
    void UpdateButtonState(UButton* Button, UImage* Icon, bool bIsHovered);
    void SetupIcons();
};