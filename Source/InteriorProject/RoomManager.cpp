#include "RoomManager.h"
#include "Components/RoomGeneratorComponent.h"
#include "ProceduralMeshComponent.h"

ARoomManager::ARoomManager()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create components
    RoomGenerator = CreateDefaultSubobject<URoomGeneratorComponent>(TEXT("RoomGenerator"));
    FloorMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("FloorMesh"));

    RootComponent = FloorMeshComponent;
    
    // Setup mesh component
    FloorMeshComponent->bUseAsyncCooking = true;
    FloorMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    bVisualizationEnabled = true;
}

void ARoomManager::BeginPlay()
{
    Super::BeginPlay();

    // Bind to room generator events
    if (RoomGenerator)
    {
        RoomGenerator->OnRoomGenerated.AddDynamic(this, &ARoomManager::OnRoomGenerated);
        RoomGenerator->OnRoomRemoved.AddDynamic(this, &ARoomManager::OnRoomRemoved);
    }
}

void ARoomManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ClearRooms();
    Super::EndPlay(EndPlayReason);
}

void ARoomManager::UpdateRooms()
{
    if (RoomGenerator)
    {
        RoomGenerator->DetectRooms();
        UpdateRoomVisualization();
    }
}

void ARoomManager::ClearRooms()
{
    // Notify before clearing
    for (const FRoom& Room : CurrentRooms)
    {
        OnRoomDeleted.Broadcast(Room);
    }

    CurrentRooms.Empty();
    ClearFloorMeshes();

    if (RoomGenerator)
    {
        RoomGenerator->ResetRooms();
    }
}

void ARoomManager::ToggleRoomVisualization()
{
    bVisualizationEnabled = !bVisualizationEnabled;
    UpdateRoomVisualization();
}

TArray<FRoom> ARoomManager::GetAllRooms() const
{
    return CurrentRooms;
}

bool ARoomManager::GetRoomAtLocation(const FVector& Location, FRoom& OutRoom)
{
    for (const FRoom& Room : CurrentRooms)
    {
        if (IsLocationInRoom(Location, Room))
        {
            OutRoom = Room;
            return true;
        }
    }
    return false;
}

bool ARoomManager::IsLocationInRoom(const FVector& Location, const FRoom& Room) const
{
    if (Room.Corners.Num() < 3)
        return false;

    // Ray casting algorithm to determine if point is inside polygon
    int32 CrossingCount = 0;
    FVector2D TestPoint(Location.X, Location.Y);

    for (int32 i = 0; i < Room.Corners.Num(); i++)
    {
        int32 j = (i + 1) % Room.Corners.Num();
        
        FVector2D PointI(Room.Corners[i].X, Room.Corners[i].Y);
        FVector2D PointJ(Room.Corners[j].X, Room.Corners[j].Y);

        if (((PointI.Y <= TestPoint.Y) && (PointJ.Y > TestPoint.Y)) || 
            ((PointI.Y > TestPoint.Y) && (PointJ.Y <= TestPoint.Y)))
        {
            float VecX = PointJ.X - PointI.X;
            float VecY = PointJ.Y - PointI.Y;
            float IntersectX = PointI.X + (TestPoint.Y - PointI.Y) * VecX / VecY;

            if (TestPoint.X < IntersectX)
            {
                CrossingCount++;
            }
        }
    }

    return (CrossingCount % 2) == 1;
}

void ARoomManager::OnRoomGenerated(const FRoom& GeneratedRoom)
{
    CurrentRooms.Add(GeneratedRoom);
    OnRoomCreated.Broadcast(GeneratedRoom);

    if (bVisualizationEnabled)
    {
        CreateFloorMesh(GeneratedRoom);
    }
}

void ARoomManager::OnRoomRemoved(const FRoom& RemovedRoom)
{
    CurrentRooms.RemoveAll([&](const FRoom& Room) {
        return Room.Walls == RemovedRoom.Walls;
    });
    OnRoomDeleted.Broadcast(RemovedRoom);
}

void ARoomManager::CreateFloorMesh(const FRoom& Room)
{
    if (!FloorMeshComponent || Room.Corners.Num() < 3)
        return;

    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UV0;
    TArray<FProcMeshTangent> Tangents;

    // Create vertices from room corners
    for (const FVector& Corner : Room.Corners)
    {
        Vertices.Add(FVector(Corner.X, Corner.Y, FloorHeight));
        Normals.Add(FVector(0, 0, 1));
        UV0.Add(FVector2D(Corner.X, Corner.Y) / 100.0f); // Scale UVs
        Tangents.Add(FProcMeshTangent(1, 0, 0));
    }

    // Triangulate the floor (simple fan triangulation)
    for (int32 i = 1; i < Vertices.Num() - 1; i++)
    {
        Triangles.Add(0);
        Triangles.Add(i);
        Triangles.Add(i + 1);
    }

    // Create mesh section
    int32 SectionIndex = FloorMeshComponent->GetNumSections();
    FloorMeshComponent->CreateMeshSection(SectionIndex, Vertices, Triangles, Normals, UV0, 
                                        TArray<FColor>(), Tangents, true);

    // Apply material if available
    if (FloorMaterial)
    {
        FloorMeshComponent->SetMaterial(SectionIndex, FloorMaterial);
    }
}

void ARoomManager::ClearFloorMeshes()
{
    if (FloorMeshComponent)
    {
        int32 NumSections = FloorMeshComponent->GetNumSections();
        for (int32 i = 0; i < NumSections; ++i)
        {
            FloorMeshComponent->ClearMeshSection(i);
        }
    }
}

void ARoomManager::UpdateRoomVisualization()
{
    ClearFloorMeshes();
    
    if (bVisualizationEnabled)
    {
        for (const FRoom& Room : CurrentRooms)
        {
            CreateFloorMesh(Room);
        }
    }
}

FString ARoomManager::GetRoomInfo(const FRoom& Room) const
{
    return FString::Printf(TEXT("Area: %.1f m², Perimeter: %.1f m"), 
        Room.Area / 10000.0f,  // Convert to square meters
        Room.Perimeter / 100.0f // Convert to meters
    );
}