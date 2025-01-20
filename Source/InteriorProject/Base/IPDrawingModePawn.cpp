#include "IPDrawingModePawn.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "InteriorProject/WallActor.h"
#include "InteriorProject/WindowActor.h"
#include "InteriorProject/Components/WallGeometryComponent.h"
#include "InteriorProject/Components/WallStateComponent.h"
#include "InteriorProject/UI/DrawingToolsWidget.h"
#include "InteriorProject/Utils/GridSnappingUtils.h"
#include "Kismet/GameplayStatics.h"

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
    CurrentWall = nullptr;
    SelectedWall = nullptr;
    CurrentPlacingWindow = nullptr;
    DrawingToolsWidget = nullptr;
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
    UpdateCurrentAction(DeltaTime);
}

// World Position Utils
bool AIPDrawingModePawn::GetMouseWorldPosition(FVector& OutLocation, FVector& OutDirection) const
{
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        return PC->DeprojectMousePositionToWorld(OutLocation, OutDirection);
    }
    return false;
}

FVector AIPDrawingModePawn::GetWorldPositionFromMouse() const
{
    FVector WorldLocation, WorldDirection;
    if (GetMouseWorldPosition(WorldLocation, WorldDirection))
    {
        WorldLocation.Z = 0;
        return GetSnappedLocation(WorldLocation);
    }
    return FVector::ZeroVector;
}

FVector AIPDrawingModePawn::GetUpdatedDragPosition(bool bIsStartCorner) const
{
    FVector Position = GetWorldPositionFromMouse();
    
    if (CurrentEditMode == EEditMode::WallDrawing && CurrentWall)
    {
        if (UWallGeometryComponent* Geometry = CurrentWall->GetGeometryComponent())
        {
            if (DrawingToolsWidget && DrawingToolsWidget->IsSnappingEnabled())
            {
                Position = GetSnappedLocation(Position);
            }

            if (bIsStartCorner)
            {
                return Position;
            }
        }
    }

    return Position;
}

// Snapping Functions
FVector AIPDrawingModePawn::GetSnappedLocation(const FVector& Location) const
{
    if (!DrawingToolsWidget || !DrawingToolsWidget->IsSnappingEnabled())
    {
        return Location;
    }

    FVector SnappedLocation = SnapToNearbyWalls(Location);
    
    if (SnappedLocation.Equals(Location) && ShouldSnapToGrid())
    {
        SnappedLocation = UGridSnappingUtils::SnapToGrid(Location, DrawingToolsWidget->GetGridSize());
    }

    return SnappedLocation;
}

TArray<AWallActor*> AIPDrawingModePawn::GetAllWalls() const
{
    TArray<AWallActor*> Walls;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWallActor::StaticClass(), reinterpret_cast<TArray<AActor*>&>(Walls));
    return Walls;
}

TArray<FVector> AIPDrawingModePawn::GetExistingWallPoints() const
{
    TArray<FVector> Points;
    TArray<AWallActor*> Walls = GetAllWalls();

    for (AWallActor* Wall : Walls)
    {
        if (!Wall || Wall == CurrentWall)
            continue;

        if (UWallGeometryComponent* Geometry = Wall->GetGeometryComponent())
        {
            Points.Add(Geometry->GetStart());
            Points.Add(Geometry->GetEnd());
        }
    }

    return Points;
}

TArray<AWallActor*> AIPDrawingModePawn::GetNearbyWalls(const FVector& Location, float Radius) const
{
    TArray<AWallActor*> NearbyWalls;
    TArray<AWallActor*> AllWalls = GetAllWalls();
    
    for (AWallActor* Wall : AllWalls)
    {
        if (!Wall || Wall == CurrentWall)
            continue;

        if (UWallGeometryComponent* Geometry = Wall->GetGeometryComponent())
        {
            FVector WallCenter = Geometry->GetCenter();
            if (FVector::Distance(Location, WallCenter) <= Radius)
            {
                NearbyWalls.Add(Wall);
            }
        }
    }

    return NearbyWalls;
}

FVector AIPDrawingModePawn::SnapToNearbyWalls(const FVector& Location) const
{
    if (!DrawingToolsWidget)
        return Location;

    float SnapThreshold = DrawingToolsWidget->GetSnapThreshold();
    TArray<AWallActor*> NearbyWalls = GetNearbyWalls(Location, WallSnapRadius);
    FVector BestSnappedPoint = Location;
    float BestDistance = SnapThreshold;

    // Try snapping to endpoints
    TArray<FVector> EndPoints = GetExistingWallPoints();
    for (const FVector& Point : EndPoints)
    {
        float Distance = FVector::Distance(Location, Point);
        if (Distance < BestDistance)
        {
            BestDistance = Distance;
            BestSnappedPoint = Point;
        }
    }

    // Try snapping to wall lines if no good endpoint snap was found
    if (BestSnappedPoint.Equals(Location))
    {
        for (AWallActor* Wall : NearbyWalls)
        {
            if (!Wall || Wall == CurrentWall)
                continue;

            UWallGeometryComponent* Geometry = Wall->GetGeometryComponent();
            if (!Geometry)
                continue;

            FVector LineSnap = UGridSnappingUtils::SnapToLine(
                Location,
                Geometry->GetStart(),
                Geometry->GetEnd(),
                SnapThreshold
            );

            float LineDistance = FVector::Distance(Location, LineSnap);
            if (LineDistance < BestDistance)
            {
                BestDistance = LineDistance;
                BestSnappedPoint = LineSnap;
            }
        }
    }

    return BestSnappedPoint;
}

bool AIPDrawingModePawn::ShouldSnapToGrid() const
{
    return DrawingToolsWidget && DrawingToolsWidget->IsSnappingEnabled();
}

// Input Handlers
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
    FVector WorldLocation, WorldDirection;
    if (GetMouseWorldPosition(WorldLocation, WorldDirection))
    {
        WorldLocation.Z = 0;
        WorldLocation = GetSnappedLocation(WorldLocation);

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
}

void AIPDrawingModePawn::OnLeftMouseReleased()
{
    // Handle if needed
}

void AIPDrawingModePawn::OnRightMousePressed()
{
    SetEditMode(EEditMode::None);
}

void AIPDrawingModePawn::UpdateCurrentAction(float DeltaTime)
{
    switch (CurrentEditMode)
    {
        case EEditMode::WallDrawing:
            if (CurrentWall)
            {
                FVector UpdatedPosition = GetUpdatedDragPosition(false);
                
                if (UWallGeometryComponent* Geometry = CurrentWall->GetGeometryComponent())
                {
                    Geometry->UpdateEndPoint(UpdatedPosition);
                }
            }
            break;

        case EEditMode::WindowPlacement:
            HandleWindowPlacement();
            break;

        default:
            break;
    }
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

void AIPDrawingModePawn::StartRectangleWallDrawing()
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
            FVector WorldLocation = GetWorldPositionFromMouse();
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

    FVector UpdatedPosition = GetWorldPositionFromMouse();
    CurrentPlacingWindow->UpdatePlacement(UpdatedPosition);
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
            break;

        case EEditMode::WindowPlacement:
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