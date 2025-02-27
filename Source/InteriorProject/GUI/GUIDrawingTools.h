#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GUIDrawingTools.generated.h"

class UGUIDetect;
class UGUIPlaceable;
class UGUIDrawingField;
class UButton;
class UImage;
class UToggleButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSnappingToggled, bool, bEnabled);


/**
 * Widget that provides drawing tools for placing walls, windows, and gates in top-down mode
 */
UCLASS()
class INTERIORPROJECT_API UGUIDrawingTools : public UUserWidget
{
    GENERATED_BODY()

public:
    
    // Set drawing field reference
    UFUNCTION(BlueprintCallable)
    void SetDrawingField(UGUIDrawingField* InDrawingField);

    FORCEINLINE bool IsSnappingEnabled() const { return bSnappingEnabled; }

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnSnappingToggled OnSnappingToggled;

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(EditDefaultsOnly, Category = "Classes|UI")
    TSubclassOf<UGUIPlaceable> GUIWindowClass;

    UPROPERTY(EditDefaultsOnly, Category = "Classes|UI")
    TSubclassOf<UGUIPlaceable> GUIGateClass;

    /** UI Elements - Only creation tools */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* DrawWallButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* DrawRectangleWallButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* AddWindowButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* AddGateButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* ToggleSnappingButton;

    /** Button Icons */
    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    UTexture2D* DrawWallIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    UTexture2D* DrawRectangleWallIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    UTexture2D* WindowIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    UTexture2D* GateIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Appearance")
    UTexture2D* SnappingIcon;

    /** Icon Images */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* DrawWallImage;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* DrawRectangleWallImage;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* WindowImage;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* GateImage;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* SnappingImage;

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

    UFUNCTION()
    void OnToggleSnappingClicked();

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
    UFUNCTION()
    void OnToggleSnappingHovered();
    UFUNCTION()
    void OnToggleSnappingUnhovered();

    /** The owning pawn reference */
    UPROPERTY()
    UGUIDrawingField* DrawingField;

    bool bSnappingEnabled = true;

    /** Visual feedback helpers */
    void UpdateButtonState(UButton* Button, UImage* Icon, bool bIsHovered);
    void SetupIcons();
};