// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GUIDrawingTools.h"
#include "Blueprint/UserWidget.h"
#include "InteriorProject/Enums/Enums.h"
#include "GUIWall.generated.h"

class APlaceableActor;
class UGUIDetect;
class UOverlay;
class AWallDynamic;
class UButton;
class UGUIMeasurement;
class UVerticalBox;
class USizeBox;
class UGUIDrawingField;

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnEnterLeave, UGUIWall*, Wall, bool, bIsMouseEnter, float, MousePosition);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDragEnded);

/**
 * 
 */
UCLASS()
class INTERIORPROJECT_API UGUIWall : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativePreConstruct() override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void UpdateWall();

	UFUNCTION()
	void UpdateWallPosition(const FVector2D& MousePosition);

	UFUNCTION()
	void UpdateWallEnd(const FVector2D& Position);

	UFUNCTION()
	void UpdateWallLength(const FText& Text, ETextCommit::Type CommitMethod);

	void UpdateWallVisual();
	
	void UpdateWallMeasurements();

	UFUNCTION()
	void OnRightHandleClicked();
	
	UFUNCTION()
	void OnLeftHandleClicked();
	
	UFUNCTION()
	void StartUpdateWallEnd();
	
	UFUNCTION()
	void FinishUpdateWallEnd();

	UFUNCTION()
	void FinishCreateWall();

	UFUNCTION()
	void DestroyWall();

	UFUNCTION()
	void MouseEnter(const float& MousePosition);

	UFUNCTION()
	void MouseLeave();

	UFUNCTION()
	void WallDrag(bool bIsStart);

	void CreateWallVisual();

	void DestroyWallVisual();
	
	UFUNCTION()
	void LeftHandleDrag(bool bIsStart);
	
	UFUNCTION()
	void RightHandleDrag(bool bIsStart);

	UFUNCTION()
	void HandleMoodChange(EDrawingTools NewMode);
	
	UFUNCTION()
	void DeSelect();

	UFUNCTION()
	void Select();

protected:

	
public:
	
	void Init(UGUIDrawingField* GUIDrawingField);
	
	UFUNCTION()
	void StartCreateWall(FVector2D Position);
	
	void ShowSnappingFeedback(bool bIsSnapped);


	FORCEINLINE UOverlay* GetWallOverlay() const { return WallOverlay; }

	FORCEINLINE float GetWallLength() const { return Length; }

	FORCEINLINE AWallDynamic* GetWall() const { return OwnWall; }

	/** Selection management */
	void SetSelectionState(bool bSelect);
	FORCEINLINE bool GetSelectedState() const { return bIsSelected; }
	
	void SetMeasurementsVisibility(ESlateVisibility Hidden);
	
	void ReBindWallInteractions();
	
	void SetSnapEnabled(bool bSnappingEnabled);


	FOnEnterLeave OnEnterLeave;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDragEnded OnDragEnded;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UGUIDetect* WallImage;

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UOverlay* WallOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* VerticalBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UGUIDetect* LeftHandle;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UGUIDetect* RightHandle;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USizeBox* MainWallSizeBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UGUIMeasurement* UpperWallMeasurement;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UGUIMeasurement* LowerWallMeasurement;
	
	UPROPERTY(EditDefaultsOnly, Category = "Custom")
	TSubclassOf<AWallDynamic> WallClass;

	UPROPERTY(EditAnywhere, Category = "Custom")
	FLinearColor NormalColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Custom")
	FLinearColor SelectedColor = FLinearColor::Green;

private:

	UPROPERTY()
	FVector2D StartPosition;

	UPROPERTY()
	FVector2D EndPosition;

	//FVector2D ReleativeMousePosition;

	// Owning widget
	UPROPERTY()
	UGUIDrawingField* DrawingField;

	UPROPERTY()
	AWallDynamic* OwnWall;

	bool bIsLeftSide = false;

	bool bIsSelected = false;
	
	// Degrees
	UPROPERTY(EditAnywhere)
	float Angle;

	UPROPERTY(EditAnywhere)
	float Length;

	UPROPERTY(EditAnywhere)
	float Thickness = 20.0f;

	UPROPERTY(EditAnywhere)
	float WallHeight = 300.0f;

	bool bEnableSnapping = true;
	float SnapAngleThreshold = 0.5f;
};
