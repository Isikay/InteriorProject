#include "RoomGeneratorComponent.h"
#include "InteriorProject/WallActor.h"
#include "InteriorProject/Components/WallGeometryComponent.h"
#include "Kismet/GameplayStatics.h"

URoomGeneratorComponent::URoomGeneratorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void URoomGeneratorComponent::BeginPlay()
{
    Super::BeginPlay();
}

void URoomGeneratorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ResetRooms();
    Super::EndPlay(EndPlayReason);
}

void URoomGeneratorComponent::DetectRooms()
{
    // Clear existing rooms
    ResetRooms();

    // Get all walls in the level
    TArray<AActor*> WallActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWallActor::StaticClass(), WallActors);

    // Try to form rooms starting from each unassigned wall
    for (AActor* Actor : WallActors)
    {
        if (AWallActor* Wall = Cast<AWallActor>(Actor))
        {
            if (!IsWallPartOfRoom(Wall))
            {
                TArray<AWallActor*> RoomWalls;
                if (TryFormRoom(Wall, RoomWalls))
                {
                    // Create and validate new room
                    FRoom NewRoom;
                    NewRoom.Walls = RoomWalls;
                    NewRoom.Corners = GetRoomCorners(RoomWalls);
                    NewRoom.Area = CalculateRoomArea(NewRoom.Corners);
                    NewRoom.Perimeter = CalculateRoomPerimeter(RoomWalls);

                    if (NewRoom.Area >= MinRoomArea)
                    {
                        DetectedRooms.Add(NewRoom);
                        NotifyRoomGenerated(NewRoom);
                    }
                }
            }
        }
    }
}

void URoomGeneratorComponent::ResetRooms()
{
    for (const FRoom& Room : DetectedRooms)
    {
        NotifyRoomRemoved(Room);
    }
    DetectedRooms.Empty();
}

bool URoomGeneratorComponent::TryFormRoom(AWallActor* StartWall, TArray<AWallActor*>& OutWalls)
{
    if (!StartWall)
        return false;

    OutWalls.Empty();
    OutWalls.Add(StartWall);

    // Keep track of walls to try
    TArray<AWallActor*> WallsToTry;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWallActor::StaticClass(), reinterpret_cast<TArray<AActor*>&>(WallsToTry));
    WallsToTry.Remove(StartWall);

    // Try to form a closed loop
    bool bFoundNewWall;
    do
    {
        bFoundNewWall = false;
        for (int32 i = WallsToTry.Num() - 1; i >= 0; --i)
        {
            if (TryAddWallToRoom(OutWalls, WallsToTry[i]))
            {
                OutWalls.Add(WallsToTry[i]);
                WallsToTry.RemoveAt(i);
                bFoundNewWall = true;
            }
        }
    } while (bFoundNewWall && OutWalls.Num() < 50); // Safety limit

    return IsValidRoom(OutWalls);
}

bool URoomGeneratorComponent::TryAddWallToRoom(const TArray<AWallActor*>& CurrentWalls, AWallActor* WallToAdd) const
{
    if (!WallToAdd || CurrentWalls.Contains(WallToAdd))
        return false;

    int32 ConnectionCount = 0;
    for (AWallActor* ExistingWall : CurrentWalls)
    {
        if (AreWallsConnected(ExistingWall, WallToAdd))
        {
            ConnectionCount++;
            if (ConnectionCount > 2) // Walls shouldn't connect to more than two others
                return false;
        }
    }

    return ConnectionCount == 1; // Should connect to exactly one existing wall
}

bool URoomGeneratorComponent::AreWallsConnected(AWallActor* Wall1, AWallActor* Wall2) const
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

float URoomGeneratorComponent::GetConnectionAngle(AWallActor* Wall1, AWallActor* Wall2, const FVector& SharedPoint) const
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

    float Angle = FMath::Acos(FVector::DotProduct(Dir1, Dir2));
    return FMath::RadiansToDegrees(Angle);
}

TArray<FVector> URoomGeneratorComponent::GetRoomCorners(const TArray<AWallActor*>& Walls) const
{
    TArray<FVector> Corners;
    if (Walls.Num() < 3)
        return Corners;

    // Get all potential corner points
    TArray<FVector> AllPoints;
    for (AWallActor* Wall : Walls)
    {
        if (UWallGeometryComponent* Geo = Wall->GetGeometryComponent())
        {
            AllPoints.Add(Geo->GetStart());
            AllPoints.Add(Geo->GetEnd());
        }
    }

    // Filter out duplicates and near-duplicates
    for (const FVector& Point : AllPoints)
    {
        bool bShouldAdd = true;
        for (const FVector& ExistingCorner : Corners)
        {
            if (FVector::Distance(Point, ExistingCorner) <= WallConnectionThreshold)
            {
                bShouldAdd = false;
                break;
            }
        }
        if (bShouldAdd)
        {
            Corners.Add(Point);
        }
    }

    return Corners;
}

float URoomGeneratorComponent::CalculateRoomArea(const TArray<FVector>& Corners) const
{
    if (Corners.Num() < 3)
        return 0.0f;

    // Use shoelace formula to calculate area
    float Area = 0.0f;
    for (int32 i = 0; i < Corners.Num(); ++i)
    {
        const FVector& Current = Corners[i];
        const FVector& Next = Corners[(i + 1) % Corners.Num()];
        Area += (Current.X * Next.Y - Next.X * Current.Y);
    }

    return FMath::Abs(Area) * 0.5f;
}

float URoomGeneratorComponent::CalculateRoomPerimeter(const TArray<AWallActor*>& Walls) const
{
    float Perimeter = 0.0f;
    for (AWallActor* Wall : Walls)
    {
        if (UWallGeometryComponent* Geo = Wall->GetGeometryComponent())
        {
            Perimeter += Geo->GetLength();
        }
    }
    return Perimeter;
}

bool URoomGeneratorComponent::IsValidRoom(const TArray<AWallActor*>& Walls) const
{
    if (Walls.Num() < 3)
        return false;

    // Check if each wall is connected to exactly two other walls
    for (AWallActor* Wall : Walls)
    {
        int32 ConnectionCount = 0;
        for (AWallActor* OtherWall : Walls)
        {
            if (Wall != OtherWall && AreWallsConnected(Wall, OtherWall))
            {
                ConnectionCount++;
            }
        }
        if (ConnectionCount != 2)
            return false;
    }

    // Calculate and check area
    TArray<FVector> Corners = GetRoomCorners(Walls);
    float Area = CalculateRoomArea(Corners);
    return Area >= MinRoomArea;
}

bool URoomGeneratorComponent::IsWallPartOfRoom(AWallActor* Wall) const
{
    for (const FRoom& Room : DetectedRooms)
    {
        if (Room.Walls.Contains(Wall))
        {
            return true;
        }
    }
    return false;
}

void URoomGeneratorComponent::NotifyRoomGenerated(const FRoom& Room)
{
    OnRoomGenerated.Broadcast(Room);
}

void URoomGeneratorComponent::NotifyRoomRemoved(const FRoom& Room)
{
    OnRoomRemoved.Broadcast(Room);
}