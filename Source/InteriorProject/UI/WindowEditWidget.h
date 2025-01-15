#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WindowEditWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class AWindowActor;

UCLASS()
class INTERIORPROJECT_API UWindowEditWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // Widget Components
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* ResizeHandleTopLeft;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* ResizeHandleTopRight;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* ResizeHandleBottomLeft;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* ResizeHandleBottomRight;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* WindowPreview;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* SizeText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* DeleteButton;

public:
    void SetTargetWindow(AWindowActor* Window);
    void UpdateWidgetPosition();
    void SetIsValidPlacement(bool bValid);

    // Utility
    void UpdateSizeText();
    bool IsInViewport() const;

private:
    UPROPERTY()
    AWindowActor* TargetWindow;

    // Event Handlers
    UFUNCTION()
    void OnResizeHandlePressed();
    
    UFUNCTION()
    void OnResizeHandleReleased();
    
    UFUNCTION()
    void OnDeleteButtonClicked();

    // State
    bool bIsResizing;
    FVector2D ResizeStartPosition;
    FVector2D InitialSize;

    
};