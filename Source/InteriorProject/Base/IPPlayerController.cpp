#include "IPPlayerController.h"
#include "IPDrawingModePawn.h"
#include "IPSpectatorPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InteriorProject/UI/CameraControlsWidget.h"

AIPPlayerController::AIPPlayerController()
{
    // Setup default controller settings
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    
    // Initialize mode
    CurrentPawnMode = EPawnMode::None;
    
}

void AIPPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Add Input Mapping Context
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }

    // Setup initial state
    SetupPawns();
    SetupUI();

    // Set initial pawn mode
    if (IPDrawingPawn)
    {
        Possess(IPDrawingPawn);
        SetPawnMode(EPawnMode::TopDown);
    }
}

void AIPPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        // Bind the quick switch camera action
        if (QuickSwitchCameraAction)
        {
            EnhancedInputComponent->BindAction(QuickSwitchCameraAction, ETriggerEvent::Triggered, this, &AIPPlayerController::OnQuickSwitchCamera);
        }
    }
}

void AIPPlayerController::SetupPawns()
{
    FTransform SpawnTransform = FTransform::Identity;
    FActorSpawnParameters SpawnParams;

    if(AIPDrawingModePawn* DrawingPawn = Cast<AIPDrawingModePawn>(GetPawn()))
    {
        IPDrawingPawn = DrawingPawn;
    }
    else
    {
        // Spawn drawing pawn
        if (DrawingPawnClass)
        {
            IPDrawingPawn = GetWorld()->SpawnActor<AIPDrawingModePawn>(
                DrawingPawnClass, 
                SpawnTransform, 
                SpawnParams
            );
        }
    }
    
    // Spawn spectator pawn
    if (SpectatorPawnClass)
    {
        IPSpectatorPawn = GetWorld()->SpawnActor<AIPSpectatorPawn>(
            SpectatorPawnClass, 
            SpawnTransform, 
            SpawnParams
        );
    }
}

void AIPPlayerController::SetupUI()
{
    if (CameraControlsWidgetClass)
    {
        CameraControlsWidget = CreateWidget<UCameraControlsWidget>(this, CameraControlsWidgetClass);
        if (CameraControlsWidget)
        {
            CameraControlsWidget->AddToViewport();
        }
    }
}

void AIPPlayerController::SwitchTo2DMode()
{
    if (IPDrawingPawn)
    {
        UnPossess();
        Possess(IPDrawingPawn);
        SetPawnMode(EPawnMode::TopDown);
    }
}

void AIPPlayerController::SwitchTo3DMode()
{
    UE_LOG( LogTemp, Warning, TEXT("AIPPlayerController::SwitchTo3DMode") );
    if (IPSpectatorPawn)
    {
        UnPossess();
        Possess(IPSpectatorPawn);
        SetPawnMode(EPawnMode::FirstPerson);
    }
}

void AIPPlayerController::SetPawnMode(EPawnMode NewMode)
{
   if (CurrentPawnMode != NewMode)
    {
        CurrentPawnMode = NewMode;
        OnPawnModeChanged.Broadcast(NewMode);
    }
}

void AIPPlayerController::OnQuickSwitchCamera()
{
    // Toggle between 2D and 3D modes
    if (CurrentPawnMode == EPawnMode::TopDown)
    {
        SwitchTo3DMode();
    }
    else
    {
        SwitchTo2DMode();
    }
}