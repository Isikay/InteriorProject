#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IPPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnModeChanged, EPawnMode, NewMode);

class AIPDrawingModePawn;
class AIPSpectatorPawn;
class UCameraControlsWidget;
class UInputAction;
class UInputMappingContext;

UCLASS()
class INTERIORPROJECT_API AIPPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AIPPlayerController();

    // Mode Change Delegate
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnPawnModeChanged OnPawnModeChanged;

    // Pawn Access
    UFUNCTION(BlueprintPure, Category = "Pawns")
    AIPDrawingModePawn* GetIPDrawingPawn() const { return IPDrawingPawn; }

    UFUNCTION(BlueprintPure, Category = "Pawns")
    AIPSpectatorPawn* GetIPSpectatorPawn() const { return IPSpectatorPawn; }

    // Mode Getters
    UFUNCTION(BlueprintPure, Category = "Camera")
    EPawnMode GetCurrentPawnMode() const { return CurrentPawnMode; }

    // Camera Control
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SwitchTo2DMode();

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SwitchTo3DMode();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // Input Mapping Context
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputMappingContext* DefaultMappingContext;

    // Input Actions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* QuickSwitchCameraAction;

    // Pawn Classes
    UPROPERTY(EditDefaultsOnly, Category = "Pawns")
    TSubclassOf<AIPDrawingModePawn> DrawingPawnClass;
    
    UPROPERTY(EditDefaultsOnly, Category = "Pawns")
    TSubclassOf<AIPSpectatorPawn> SpectatorPawnClass;

    // UI Class
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UCameraControlsWidget> CameraControlsWidgetClass;

private:
    // Current Mode
    UPROPERTY()
    EPawnMode CurrentPawnMode;

    // Pawns
    UPROPERTY()
    AIPDrawingModePawn* IPDrawingPawn;
    
    UPROPERTY()
    AIPSpectatorPawn* IPSpectatorPawn;

    // UI
    UPROPERTY()
    UCameraControlsWidget* CameraControlsWidget;

    // Setup Functions
    void SetupPawns();
    void SetupUI();
    void SetPawnMode(EPawnMode NewMode);

    // Input Handlers
    void OnQuickSwitchCamera();
};