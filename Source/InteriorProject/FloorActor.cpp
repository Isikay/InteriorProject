// Fill out your copyright notice in the Description page of Project Settings.
#include "FloorActor.h"
#include "WallActor.h"
#include "Components/WallGeometryComponent.h"
#include "Components/WallStateComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ConnectedWallsWidget.h"


// Sets default values
AFloorActor::AFloorActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);
   
    CurrentWall = nullptr;
    SelectedWall = nullptr;
    MovingWall = nullptr;
}

void AFloorActor::Dragg(const FVector& DraggedDistance)
{
    SetActorLocation(GetActorLocation() - DraggedDistance);
}

// Called when the game starts or when spawned
void AFloorActor::BeginPlay()
{
	Super::BeginPlay();

    if(StaticMeshComponent)
    {
        StaticMeshComponent->OnClicked.AddDynamic(this, &AFloorActor::OnFloorClicked);
    }

    UMaterialInstanceDynamic* MaterialInstanceDynamic = StaticMeshComponent->CreateDynamicMaterialInstance(0);
    /*MaterialInstanceDynamic->SetScalarParameterValue("UV Tiling U Text", 2.0f);
    MaterialInstanceDynamic->SetScalarParameterValue("UV Tiling V Text", 2.0f);
    MaterialInstanceDynamic->SetScalarParameterValue("UV Tiling U BP", 1.0f);
    MaterialInstanceDynamic->SetScalarParameterValue("UV Tiling V BP", 1.0f);*/
    MaterialInstanceDynamic->SetScalarParameterValue("V Tiling", 100.0f);
    MaterialInstanceDynamic->SetScalarParameterValue("U Tiling", 100.0f);
    
}

void AFloorActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CleanupWalls();
    Super::EndPlay(EndPlayReason);
}

// Wall Creation
void AFloorActor::StartOrEndWallDrawing(const FVector& Location)
{
    //log
    UE_LOG(LogTemp, Warning, TEXT("StartOrEndWallDrawing"));
    
    //  Check if we are already drawing a wall
    if (CurrentWall)
    {
        CurrentWall = nullptr;
    }

    // Create new wall
    if (WallActorClass)
    {
        
        CurrentWall = GetWorld()->SpawnActor<AWallActor>(WallActorClass, FTransform(Location));
        if (CurrentWall)
        {
            CurrentWall->OnDestroyed.AddDynamic(this, &AFloorActor::OnWallDestroyed);
            CurrentWall->OnCornerHovered.AddDynamic(this, &AFloorActor::OnCornerHovered);
            CurrentWall->OnCornerClicked.AddDynamic(this, &AFloorActor::OnCornerClicked);
            CurrentWall->StartDrawing(Location, DrawingModePawn);
            HandleWallDrawingStarted(CurrentWall);
        }
    }
}

void AFloorActor::EndWallDrawing()
{
    if (!CurrentWall)
        return;
        
    AWallActor* WallToEnd = CurrentWall;
    CurrentWall = nullptr; // Clear reference first to prevent cycles
    
    if (WallToEnd->GetStateComponent())
    {
        WallToEnd->GetStateComponent()->SetState(EWallState::Completed);
    }
    HandleWallDrawingEnded(WallToEnd);
}

void AFloorActor::CancelWallDrawing()
{
    if (CurrentWall)
    {
        AWallActor* WallToDestroy = CurrentWall;
        CurrentWall = nullptr;
        WallToDestroy->Destroy();
    }
}

// Wall Movement
void AFloorActor::StartWallMove(AWallActor* Wall)
{
    /*if (!Wall)
        return;
        
    MovingWall = Wall;
    ConnectedWalls = GetConnectedWalls(Wall);
    
    // Store initial positions
    InitialPositions.Empty();
    InitialPositions.Add(Wall->GetActorLocation());
    
    for (AWallActor* Connected : ConnectedWalls)
    {
        InitialPositions.Add(Connected->GetActorLocation());
    }
    
    MoveStartLocation = Wall->GetActorLocation();
    
    // Start wall movement
    MovingWall->StartMove();
    for (AWallActor* Connected : ConnectedWalls)
    {
        Connected->StartMove();
    }*/
}

void AFloorActor::UpdateWallMove(const FVector& NewLocation)
{
    /*if (!MovingWall)
        return;
        
    FVector DeltaMove = NewLocation - MoveStartLocation;
    UpdateConnectedWallPositions(DeltaMove);*/
}

void AFloorActor::EndWallMove()
{
    /*if (MovingWall)
    {
        MovingWall->EndMove();
        for (AWallActor* Connected : ConnectedWalls)
        {
            Connected->EndMove();
        }
    }

    MovingWall = nullptr;
    ConnectedWalls.Empty();
    InitialPositions.Empty();*/
}

void AFloorActor::UpdateConnectedWallPositions(const FVector& DeltaMove)
{
    /*if (!MovingWall)
        return;

    // Move primary wall
    MovingWall->UpdateMove(InitialPositions[0] + DeltaMove);
    
    // Move connected walls
    for (int32 i = 0; i < ConnectedWalls.Num(); ++i)
    {
        if (AWallActor* ConnectedWall = ConnectedWalls[i])
        {
            ConnectedWall->UpdateMove(InitialPositions[i + 1] + DeltaMove);
        }
    }*/
}

// Wall Queries
TArray<AWallActor*> AFloorActor::GetAllWalls() const
{
    TArray<AWallActor*> Walls;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), WallActorClass, reinterpret_cast<TArray<AActor*>&>(Walls));
    return Walls;
}

TArray<AWallActor*> AFloorActor::GetNearbyWalls(const FVector& Location, float Radius) const
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

TArray<FVector> AFloorActor::GetWallSnapPoints() const
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
            Points.Add(Geometry->GetCenter());
        }
    }
    
    return Points;
}

TArray<AWallActor*> AFloorActor::GetConnectedWalls(AWallActor* Wall) const
{
    TArray<AWallActor*> Result;
    if (!Wall)
        return Result;

    TArray<AWallActor*> AllWalls = GetAllWalls();
    
    for (AWallActor* OtherWall : AllWalls)
    {
        if (OtherWall != Wall && AreWallsConnected(Wall, OtherWall))
        {
            Result.Add(OtherWall);
        }
    }
    
    return Result;
}

void AFloorActor::SetEditMode(EEditMode NewMode, EEditMode OldMode)
{

}

bool AFloorActor::AreWallsConnected(AWallActor* Wall1, AWallActor* Wall2) const
{
    if (!Wall1 || !Wall2 || Wall1 == Wall2)
        return false;

    UWallGeometryComponent* Geo1 = Wall1->GetGeometryComponent();
    UWallGeometryComponent* Geo2 = Wall2->GetGeometryComponent();

    if (!Geo1 || !Geo2)
        return false;

    return FVector::Distance(Geo1->GetStart(), Geo2->GetStart()) <= WallConnectionThreshold ||
           FVector::Distance(Geo1->GetStart(), Geo2->GetEnd()) <= WallConnectionThreshold ||
           FVector::Distance(Geo1->GetEnd(), Geo2->GetStart()) <= WallConnectionThreshold ||
           FVector::Distance(Geo1->GetEnd(), Geo2->GetEnd()) <= WallConnectionThreshold;
}

float AFloorActor::GetConnectionAngle(AWallActor* Wall1, AWallActor* Wall2, const FVector& SharedPoint) const
{
    if (!Wall1 || !Wall2)
        return 0.0f;

    UWallGeometryComponent* Geo1 = Wall1->GetGeometryComponent();
    UWallGeometryComponent* Geo2 = Wall2->GetGeometryComponent();

    if (!Geo1 || !Geo2)
        return 0.0f;

    FVector Dir1, Dir2;

    // Get wall directions based on shared point
    if (FVector::Distance(SharedPoint, Geo1->GetStart()) <= WallConnectionThreshold)
        Dir1 = (Geo1->GetEnd() - Geo1->GetStart()).GetSafeNormal();
    else
        Dir1 = (Geo1->GetStart() - Geo1->GetEnd()).GetSafeNormal();

    if (FVector::Distance(SharedPoint, Geo2->GetStart()) <= WallConnectionThreshold)
        Dir2 = (Geo2->GetEnd() - Geo2->GetStart()).GetSafeNormal();
    else
        Dir2 = (Geo2->GetStart() - Geo2->GetEnd()).GetSafeNormal();

    float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Dir1, Dir2)));
    return Angle;
}

void AFloorActor::HandleWallDrawingStarted(AWallActor* Wall)
{
    OnWallDrawingStarted.Broadcast(Wall);
}

void AFloorActor::HandleWallDrawingEnded(AWallActor* Wall)
{
    OnWallDrawingEnded.Broadcast(Wall);
}

void AFloorActor::CleanupWalls()
{
    if (CurrentWall)
    {
        CurrentWall->Destroy();
        CurrentWall = nullptr;
    }
    
    if (MovingWall)
    {
        EndWallMove();
    }
}

void AFloorActor::OnWallDestroyed(AActor* DestroyedActor)
{
    AWallActor* WallActor = Cast<AWallActor>(DestroyedActor);
    if (!WallActor)
        return;

    if (WallActor == CurrentWall)
    {
        CurrentWall = nullptr;
    }
    if (WallActor == SelectedWall)
    {
        SelectedWall = nullptr;
    }
    if (WallActor == MovingWall)
    {
        EndWallMove();
    }

    ConnectedWalls.Remove(WallActor);
}

void AFloorActor::OnCornerHovered(AWallActor* Wall, bool bIsStartCorner, bool bIsHovered)
{
    // Snap to corner
    if (bIsHovered)
    {
        if (CurrentWall && Wall!=CurrentWall)
        {
            UWallGeometryComponent* CurrentGeo = CurrentWall->GetGeometryComponent();
            UWallGeometryComponent* WallGeo = Wall->GetGeometryComponent();

            // Store corner for snapping
            HoveredCorner = FWallCorner{Wall, bIsStartCorner};
            
            if (CurrentGeo && WallGeo)
            {
                FVector SnapPoint = bIsStartCorner ? WallGeo->GetStart() : WallGeo->GetEnd();
                CurrentGeo->UpdateEndPoint(SnapPoint);
            }
        }
    }
    else
    {
        HoveredCorner = FWallCorner{nullptr, false};
    }
}

void AFloorActor::OnCornerClicked(AWallActor* Wall, bool bIsStartCorner)
{
    /*// Update wall drawing
    if(CurrentWall)
    {
        UWallGeometryComponent* CurrentGeo = CurrentWall->GetGeometryComponent();
        UWallGeometryComponent* WallGeo = Wall->GetGeometryComponent();

        if (CurrentGeo && WallGeo)
        {
            FVector SnapPoint = bIsStartCorner ? WallGeo->GetStart() : WallGeo->GetEnd();
            CurrentGeo->UpdateEndPoint(SnapPoint);
        }
        
        // If 
        
    }
    else
    {
        CurrentWall = Wall;
    }*/
}

void AFloorActor::OnFloorClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed)
{
    if(CurrentWall)
    {
        
    }
}
