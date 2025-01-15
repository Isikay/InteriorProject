#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WindowPlacementWidget.generated.h"

class UImage;
class UBorder;
class AWallActor;
class UWallGeometryComponent;
class UWallWindowComponent;

UCLASS()
class INTERIORPROJECT_API UWindowPlacementWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    // Wall interaction
    void SetTargetWall(AWallActor* Wall);
    void ClearTargetWall();
    bool HasTargetWall() const { return TargetWall != nullptr; }
    AWallActor* GetTargetWall() const { return TargetWall; }

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UBorder* MainBorder;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* ResizeHandle;

    // Visual Properties
    UPROPERTY(EditAnywhere, Category = "Appearance")
    FLinearColor ValidPlacementColor = FLinearColor(0.0f, 1.0f, 0.0f, 0.5f);

    UPROPERTY(EditAnywhere, Category = "Appearance")
    FLinearColor InvalidPlacementColor = FLinearColor(1.0f, 0.0f, 0.0f, 0.5f);

    UPROPERTY(EditAnywhere, Category = "Appearance")
    FLinearColor DefaultColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.5f);

private:
    bool bIsDragging;
    bool bIsValidPlacement;
    FVector2D InitialSize;
    FVector2D CurrentSize;
    FVector2D StartPosition;
    
    UPROPERTY()
    AWallActor* TargetWall;

    UPROPERTY()
    UWallGeometryComponent* GeometryComponent;

    UPROPERTY()
    UWallWindowComponent* WindowComponent;

    void UpdateVisuals();
    void FollowMouse(const FVector2D& MousePosition);
    void UpdateWindowSize(const FVector2D& NewSize);
    void ValidatePlacement();
    void CleanupReferences();
    
    FVector2D GetLocalPositionOnWall(const FVector2D& ScreenPosition) const;
    bool GetMousePositionOnWall(FVector2D& OutPosition) const;
};