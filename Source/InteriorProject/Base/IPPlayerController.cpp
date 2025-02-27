#include "IPPlayerController.h"
#include "IPDrawingModePawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InteriorProject/GUI/GUIDrawingField.h"
#include "InteriorProject/GUI/GUIDrawingTools.h"

AIPPlayerController::AIPPlayerController()
{
    // Setup default controller settings
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    
    // Initialize mode
    CurrentPawnMode = EPawnMode::None;
    
    bEnableClickEvents = true;
    bEnableMouseOverEvents  = true;
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
    //SetupPawns();
    SetupUI();
    SetPawnMode(EPawnMode::TopDown);
    /*// Set initial pawn mode
    if (IPDrawingPawn)
    {
        Possess(IPDrawingPawn);
        SetPawnMode(EPawnMode::TopDown);
    }*/

   
}

void AIPPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(QuickSwitchCameraAction, ETriggerEvent::Completed, this, &AIPPlayerController::OnQuickSwitchCamera);
        EnhancedInputComponent->BindAction(LeftMouseButtonAction, ETriggerEvent::Completed, this, &AIPPlayerController::LeftMouseButton);
    }
}

void AIPPlayerController::SetupPawns()
{
    /*FTransform SpawnTransform = FTransform::Identity;
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
    }*/
}

void AIPPlayerController::SetupUI()
{
    if (DrawingFieldClass && DrawingToolsClass)
    {
        DrawingField = CreateWidget<UGUIDrawingField>(this, DrawingFieldClass);
        DrawingTools = CreateWidget<UGUIDrawingTools>(this, DrawingToolsClass);

        if (DrawingField && DrawingTools)
        {
            DrawingField->AddToViewport();
            DrawingTools->AddToViewport();
            DrawingTools->SetDrawingField(DrawingField);
        }
    }
}

void AIPPlayerController::SwitchTo2DMode()
{
    if (DrawingField && DrawingTools)
    {
        DrawingField->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        DrawingTools->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        SetPawnMode(EPawnMode::TopDown);
    }
}

void AIPPlayerController::SwitchTo3DMode()
{
    if (DrawingField && DrawingTools)
    {
        DrawingField->SetVisibility(ESlateVisibility::Collapsed);
        DrawingTools->SetVisibility(ESlateVisibility::Collapsed);
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
    if (CurrentPawnMode == EPawnMode::TopDown)
    {
        SwitchTo3DMode();
    }
    else
    {
        SwitchTo2DMode();
    }
}

void AIPPlayerController::LeftMouseButton()
{
    if( CurrentPawnMode == EPawnMode::FirstPerson )
    {
        OnLeftMouseButton.Broadcast();
    }
}
