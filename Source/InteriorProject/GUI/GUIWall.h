#pragma once

#include "CoreMinimal.h"
#include "GUIDrawingTools.h"
#include "Blueprint/UserWidget.h"
#include "InteriorProject/Enums/Enums.h"
#include "GUIWall.generated.h"

class UGUIWallHandle;
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

private:
	// Connect handles to nearby walls for joining walls at endpoints
	void ConnectHandlesToNearbyWalls();
	void ConnectHandleToNearbyWalls(const FVector2D& Position, UGUIWallHandle* Handle, const TArray<UWidget*>& AllWidgets);
	
#if WITH_EDITOR
	// Debug helper for visualizing handle connections
	void DebugVisualizeHandleConnections();
#endif

	// Handle için gerekli olan pozisyon bilgileri
	UPROPERTY()
	FVector2D StartPosition;

	UPROPERTY()
	FVector2D EndPosition;

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

	// Visual snapping indicator
	UPROPERTY()
	bool bEndpointSnapped = false;

	bool bIsPermanentWall = false;

	bool bCanSplit = false;

	float SplitPosition;

	// Timer handle for resetting visual feedback
	FTimerHandle SnapVisualFeedbackTimer;

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UOverlay* WallOverlay;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* VerticalBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UGUIWallHandle* LeftHandle;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UGUIWallHandle* RightHandle;

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

	UPROPERTY(EditAnywhere, Category = "Snapping")
	float EndpointSnapThreshold = 15.0f;  // Distance in pixels for endpoint snapping

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowSnapPoints = false;
	
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDebugConnections = false;

public:
	
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
	void HandleMoodChange(EDrawingTools NewMode);
	
	UFUNCTION()
	void DeSelect();

	UFUNCTION()
	void Select();

	/**
	 * Check for nearby wall endpoints for snapping
	 * @param Position The current position to check against
	 * @param IsStartPoint Whether we're checking for the start or end point
	 * @return True if snapping occurred
	 */
	bool CheckForEndpointSnapping(FVector2D& Position, bool IsStartPoint);

	/**
	 * Find a snap point near the given position
	 * @param Position The position to check (will be modified if snapping occurs)
	 * @return True if a snap point was found
	 */
	bool FindSnapPointNearPosition(FVector2D& Position);

	void Init(UGUIDrawingField* GUIDrawingField);
	
	UFUNCTION()
	void StartCreateWall(FVector2D Position);
	
	void ShowSnappingFeedback(bool bIsSnapped);
	
	void HandleDrag(bool bIsDragStart, bool bIsLeft);

	FORCEINLINE UOverlay* GetWallOverlay() const { return WallOverlay; }

	FORCEINLINE float GetWallLength() const { return Length; }

	FORCEINLINE AWallDynamic* GetWall() const { return OwnWall; }

	// Handle'lar için pozisyon erişim fonksiyonları
	FORCEINLINE FVector2D GetLeftHandlePosition() const { return StartPosition; }
	FORCEINLINE FVector2D GetRightHandlePosition() const { return EndPosition; }

	/** Selection management */
	void SetSelectionState(bool bSelect);
	FORCEINLINE bool GetSelectedState() const { return bIsSelected; }
	
	void SetMeasurementsVisibility(ESlateVisibility Hidden);
	
	void ReBindWallInteractions();

	UFUNCTION()
	void SetSnapEnabled(bool bSnappingEnabled);

	UFUNCTION()
	void CachePosition(const float& Position);
	
	UFUNCTION()
	void SplitEnter(const float& Position);

	UFUNCTION()
	void SplitLeave();

	UFUNCTION()
	void SplitWall();
	
	void SetCanSplit(bool Split);

	FOnEnterLeave OnEnterLeave;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDragEnded OnDragEnded;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UGUIDetect* WallImage;
	
};