#include "IPDrawingModePawn.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"

AIPDrawingModePawn::AIPDrawingModePawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // Setup orthographic camera
    MainCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
    MainCamera->OrthoWidth = 1024.0f;
    MainCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 1000.0f));
    MainCamera->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
    MainCamera->bUsePawnControlRotation = false;

    // Initialize state
    CurrentEditMode = EEditMode::None;
}

void AIPDrawingModePawn::BeginPlay()
{
    Super::BeginPlay();

    // Set initial position
    SetActorLocation(FVector(0.0f, 0.0f, 1000.0f));
    
    //Create timer for position update
    GetWorldTimerManager().SetTimer(TimerHandle,
        this, 
        &AIPDrawingModePawn::MousePositionUptade, 
        0.1f, 
        true, 
        0.0f);
}

void AIPDrawingModePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

    Super::EndPlay(EndPlayReason);
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

void AIPDrawingModePawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AIPDrawingModePawn::OnLeftMousePressed()
{
}

void AIPDrawingModePawn::OnLeftMouseReleased()
{
    switch (CurrentEditMode)
    {
    case EEditMode::None:
        break;
    default:
        break;
    }
   
}

void AIPDrawingModePawn::OnRightMousePressed()
{
  
}

void AIPDrawingModePawn::MousePositionUptade()
{
    OnMousePositionUpdate.Broadcast(GetWorldPositionFromMouse());
}

bool AIPDrawingModePawn::GetActorUnderMousePosition(FVector& OutPosition) const
{
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        FHitResult HitResult;
        PlayerController->GetHitResultUnderCursorByChannel(TraceChannel, true, HitResult);
        if (AActor* HitActor = HitResult.GetActor())
        {
            OutPosition = HitResult.Location;
            OutPosition.Z = 0.0f;
            return true;
        }
    }

    return false;
}

FVector AIPDrawingModePawn::GetWorldPositionFromMouse() const
{
    FVector WorldLocation = FVector::ZeroVector;
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        FVector WorldDirection;
        if(PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
        {
            WorldLocation.Z = 0;
        }
    }
    
    return WorldLocation;
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
}

void AIPDrawingModePawn::StartWallDrawing()
{
    SetEditMode(EEditMode::WallDrawing);
}

void AIPDrawingModePawn::StartWindowPlacement()
{
    
}


void AIPDrawingModePawn::SetEditMode(EEditMode NewMode)
{
    if (CurrentEditMode != NewMode)
    {
        EEditMode OldMode = CurrentEditMode;
        CurrentEditMode = NewMode;
        OnEditModeChanged.Broadcast(NewMode, OldMode);
    }
}
