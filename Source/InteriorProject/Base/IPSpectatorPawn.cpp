#include "IPSpectatorPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AIPSpectatorPawn::AIPSpectatorPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create root component if not created
    if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    }

    // Setup perspective camera
    MainCamera->ProjectionMode = ECameraProjectionMode::Perspective;
    MainCamera->FieldOfView = InitialFOV;
    MainCamera->bUsePawnControlRotation = false;
}

void AIPSpectatorPawn::BeginPlay()
{
    Super::BeginPlay();
}

void AIPSpectatorPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AIPSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Movement
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIPSpectatorPawn::Move);
        }

        // Vertical Movement
        if (MoveUpAction)
        {
            EnhancedInputComponent->BindAction(MoveUpAction, ETriggerEvent::Triggered, this, &AIPSpectatorPawn::MoveUp);
        }

        // Camera Look
        if (LookAction)
        {
            EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AIPSpectatorPawn::Look);
        }

        // Camera Rotation Toggle
        if (CameraRotateAction)
        {
            EnhancedInputComponent->BindAction(CameraRotateAction, ETriggerEvent::Started, this, &AIPSpectatorPawn::StartCameraRotation);
            EnhancedInputComponent->BindAction(CameraRotateAction, ETriggerEvent::Completed, this, &AIPSpectatorPawn::StopCameraRotation);
        }
    }
}

void AIPSpectatorPawn::Move(const FInputActionValue& Value)
{
    if (!Controller || !bIsRotating)
        return;

    const FVector2D MovementVector = Value.Get<FVector2D>();

    // Forward/Backward movement
    if (MovementVector.Y != 0.0f)
    {
        const FVector ForwardVector = GetActorForwardVector();
        AddActorWorldOffset(ForwardVector * MovementVector.Y * MovementSpeed * GetWorld()->GetDeltaSeconds());
    }

    // Right/Left movement
    if (MovementVector.X != 0.0f)
    {
        const FVector RightVector = GetActorRightVector();
        AddActorWorldOffset(RightVector * MovementVector.X * MovementSpeed * GetWorld()->GetDeltaSeconds());
    }
}

void AIPSpectatorPawn::MoveUp(const FInputActionValue& Value)
{
    if (!Controller || !bIsRotating)
        return;

    const float UpValue = Value.Get<float>();
    if (UpValue != 0.0f)
    {
        AddActorWorldOffset(FVector(0.0f, 0.0f, UpValue * VerticalMovementSpeed * GetWorld()->GetDeltaSeconds()));
    }
}

void AIPSpectatorPawn::Look(const FInputActionValue& Value)
{
    if (!Controller || !bIsRotating)
        return;

    const FVector2D LookAxisVector = Value.Get<FVector2D>();

    // Yaw rotation (around Z axis)
    if (LookAxisVector.X != 0.0f)
    {
        AddActorWorldRotation(FRotator(0.0f, LookAxisVector.X * RotationSpeed, 0.0f));
    }

    // Pitch rotation (around Y axis)
    if (LookAxisVector.Y != 0.0f)
    {
        float NewPitch = MainCamera->GetRelativeRotation().Pitch + LookAxisVector.Y * RotationSpeed;
        NewPitch = FMath::Clamp(NewPitch, MinPitchAngle, MaxPitchAngle);
        
        FRotator NewRotation = MainCamera->GetRelativeRotation();
        NewRotation.Pitch = NewPitch;
        MainCamera->SetRelativeRotation(NewRotation);
    }
}

void AIPSpectatorPawn::StartCameraRotation()
{
    bIsRotating = true;

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }
}

void AIPSpectatorPawn::StopCameraRotation()
{
    bIsRotating = false;

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        PC->bShowMouseCursor = true;
        PC->SetInputMode(FInputModeGameAndUI());
    }
}