#include "IPDrawingModePawn.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "InteriorProject/WallActor.h"
#include "InteriorProject/WindowActor.h"
#include "InteriorProject/Components/WallGeometryComponent.h"
#include "InteriorProject/Components/WallStateComponent.h"
#include "InteriorProject/UI/DrawingToolsWidget.h"


AIPDrawingModePawn::AIPDrawingModePawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // Setup orthographic camera
    MainCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
    MainCamera->OrthoWidth = 1024.0f;
    MainCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 1000.0f));  // Position camera high enough
    MainCamera->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
    MainCamera->bUsePawnControlRotation = false;

    // Initialize state
    CurrentEditMode = EEditMode::None;
}

void AIPDrawingModePawn::BeginPlay()
{
    Super::BeginPlay();

    // Set initial position of the pawn
    SetActorLocation(FVector(0.0f, 0.0f, 1000.0f));

    if (DrawingToolsWidgetClass)
    {
        DrawingToolsWidget = CreateWidget<UDrawingToolsWidget>(GetWorld()->GetFirstPlayerController(), DrawingToolsWidgetClass);
        if (DrawingToolsWidget)
        {
            DrawingToolsWidget->AddToViewport();
        }
    }
}

void AIPDrawingModePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CleanupWall();
    CleanupWindow();

    if (DrawingToolsWidget)
    {
        DrawingToolsWidget->RemoveFromParent();
        DrawingToolsWidget = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

void AIPDrawingModePawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateCurrentAction(DeltaTime);
}

void AIPDrawingModePawn::UpdateCurrentAction(float DeltaTime)
{
    switch (CurrentEditMode)
    {
        case EEditMode::WindowPlacement:
            HandleWindowPlacement();
            break;
        default:
            break;
    }
}

void AIPDrawingModePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIPDrawingModePawn::Move);
        EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AIPDrawingModePawn::Zoom);
        EnhancedInputComponent->BindAction(LeftMouseAction, ETriggerEvent::Started, this, &AIPDrawingModePawn::OnLeftMousePressed);
        EnhancedInputComponent->BindAction(LeftMouseAction, ETriggerEvent::Completed, this, &AIPDrawingModePawn::OnLeftMouseReleased);
        EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Started, this, &AIPDrawingModePawn::OnRightMousePressed);
    }
}

void AIPDrawingModePawn::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();
    
    if (MovementVector.Y != 0.0f)
    {
        AddActorWorldOffset(FVector(MovementVector.Y * MovementSpeed * GetWorld()->GetDeltaSeconds(), 0.0f, 0.0f));
    }

    if (MovementVector.X != 0.0f)
    {
        AddActorWorldOffset(FVector(0.0f, MovementVector.X * MovementSpeed * GetWorld()->GetDeltaSeconds(), 0.0f));
    }
}

void AIPDrawingModePawn::Zoom(const FInputActionValue& Value)
{
    if (!MainCamera)
        return;

    const float ZoomValue = Value.Get<float>();
    if (ZoomValue != 0.0f)
    {
        float NewOrthoWidth = MainCamera->OrthoWidth + (ZoomValue * -ZoomSpeed);
        NewOrthoWidth = FMath::Clamp(NewOrthoWidth, MinZoom, MaxZoom);
        MainCamera->OrthoWidth = NewOrthoWidth;
    }
    if(SelectedWall)
    {
        SelectedWall->UpdateMeaseurementWidget();
    }
    if (CurrentWall)
    {
        CurrentWall->UpdateMeaseurementWidget();
    }
}

void AIPDrawingModePawn::OnLeftMousePressed()
{
    FVector WorldLocation;
    FVector WorldDirection;
    Cast<APlayerController>(Controller)->DeprojectMousePositionToWorld(WorldLocation,WorldDirection);
    WorldLocation.Z=0;

    switch (CurrentEditMode)
    {
        case EEditMode::WallDrawing:
            if (!CurrentWall)
            {
                CurrentWall = SpawnWall(WorldLocation);
                if (CurrentWall)
                {
                    CurrentWall->StartDrawing(WorldLocation);
                }
            }
            break;

        case EEditMode::WindowPlacement:
            EndWindowPlacement();
            break;

        default:
            break;
    }
}

void AIPDrawingModePawn::OnLeftMouseReleased()
{
    // Handle if needed
}

void AIPDrawingModePawn::OnRightMousePressed()
{
    SetEditMode(EEditMode::None);
}

AWallActor* AIPDrawingModePawn::SpawnWall(const FVector& Location)
{
    if (!WallActorClass)
        return nullptr;

    AWallActor* NewWall = GetWorld()->SpawnActor<AWallActor>(WallActorClass, FTransform(Location), FActorSpawnParameters());
    if (NewWall)
    {
        NewWall->SubscribeToEditModeChanges(this);
        NewWall->OnWallSelected.AddDynamic(this, &AIPDrawingModePawn::OnWallSelected);
    }
    
    return NewWall;
}

void AIPDrawingModePawn::StartWallDrawing()
{
    if (SelectedWall)
    {
        if (UWallStateComponent* StateComp = SelectedWall->FindComponentByClass<UWallStateComponent>())
        {
            StateComp->SetSelected(false);
        }
        SelectedWall = nullptr;
    }
    
    SetEditMode(EEditMode::WallDrawing);
}

void AIPDrawingModePawn::StartWindowPlacement()
{
    SetEditMode(EEditMode::WindowPlacement);
    
    if (WindowActorClass && !CurrentPlacingWindow)
    {
        FTransform SpawnTransform = FTransform::Identity;
        CurrentPlacingWindow = GetWorld()->SpawnActor<AWindowActor>(WindowActorClass, SpawnTransform, FActorSpawnParameters());
        
        if (CurrentPlacingWindow)
        {
            FVector WorldLocation;
            FVector WorldDirection;
            Cast<APlayerController>(Controller)->DeprojectMousePositionToWorld(WorldLocation,WorldDirection);
            WorldLocation.Z=0;
            CurrentPlacingWindow->StartPlacement(WorldLocation);
        }
    }
}

void AIPDrawingModePawn::EndWallDrawing()
{
    CurrentWall = nullptr;
    SetEditMode(EEditMode::None);
}

void AIPDrawingModePawn::CancelWallDrawing()
{
    if (CurrentWall)
    {
        CurrentWall->CancelDrawing();
        CurrentWall = nullptr;
    }
}

void AIPDrawingModePawn::CleanupWall()
{
    if (CurrentWall)
    {
        CurrentWall->Destroy();
        CurrentWall = nullptr;
    }
    
    SelectedWall = nullptr;
}

void AIPDrawingModePawn::HandleWindowPlacement()
{
    if (!CurrentPlacingWindow)
        return;

    FVector WorldLocation;
    FVector WorldDirection;
    Cast<APlayerController>(Controller)->DeprojectMousePositionToWorld(WorldLocation,WorldDirection);
    WorldLocation.Z=0;
    CurrentPlacingWindow->UpdatePlacement(WorldLocation);
}

void AIPDrawingModePawn::EndWindowPlacement()
{
    if (CurrentPlacingWindow)
    {
        CurrentPlacingWindow->FinishPlacement();
        CurrentPlacingWindow = nullptr;
    }
    SetEditMode(EEditMode::None);
}

void AIPDrawingModePawn::CancelWindowPlacement()
{
    if (CurrentPlacingWindow)
    {
        CurrentPlacingWindow->CancelPlacement();
        CurrentPlacingWindow = nullptr;
    }
}

void AIPDrawingModePawn::CleanupWindow()
{
    if (CurrentPlacingWindow)
    {
        CurrentPlacingWindow->Destroy();
        CurrentPlacingWindow = nullptr;
    }
}

void AIPDrawingModePawn::SetEditMode(EEditMode NewMode)
{
    if (CurrentEditMode != NewMode)
    {
        EEditMode OldMode = CurrentEditMode;
        CurrentEditMode = NewMode;
        HandleEditModeChange(NewMode, OldMode);
        OnEditModeChanged.Broadcast(NewMode, OldMode);
    }
}

void AIPDrawingModePawn::HandleEditModeChange(EEditMode NewMode, EEditMode OldMode)
{
    // Cleanup old mode
    switch (OldMode)
    {
        case EEditMode::WallDrawing:
            CancelWallDrawing();
            break;
        case EEditMode::WindowPlacement:
            CancelWindowPlacement();
            break;
        default:
            break;
    }

    // Setup new mode
    switch (NewMode)
    {
        case EEditMode::None:
            // Clear any selected walls when entering None mode
            if (SelectedWall)
            {
                if (UWallStateComponent* StateComp = SelectedWall->FindComponentByClass<UWallStateComponent>())
                {
                    StateComp->SetSelected(false);
                }
                SelectedWall = nullptr;
            }
            break;

        case EEditMode::WallDrawing:
            // Any specific setup for wall drawing mode
            break;

        case EEditMode::WindowPlacement:
            // Ensure we have no active wall drawing
            CancelWallDrawing();
            break;

        default:
            break;
    }
}

void AIPDrawingModePawn::OnWallSelected(AWallActor* Wall)
{
    if (SelectedWall && SelectedWall != Wall)
    {
        SelectedWall->HandleDeselection();
    }

    SelectedWall = Wall;
}

void AIPDrawingModePawn::StartRectangleWallDrawing()
{
    // Clear any existing selection
    if (SelectedWall)
    {
        if (UWallStateComponent* StateComp = SelectedWall->FindComponentByClass<UWallStateComponent>())
        {
            StateComp->SetSelected(false);
        }
        SelectedWall = nullptr;
    }
    
    SetEditMode(EEditMode::WallDrawing);
    // Additional rectangle-specific setup can be added here
}