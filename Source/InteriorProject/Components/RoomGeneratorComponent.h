#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RoomGeneratorComponent.generated.h"

class AWallActor;

USTRUCT(BlueprintType)
struct FRoom
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Room")
    TArray<AWallActor*> Walls;

    UPROPERTY(BlueprintReadOnly, Category = "Room")
    float Area;

    UPROPERTY(BlueprintReadOnly, Category = "Room")
    float Perimeter;

    UPROPERTY(BlueprintReadOnly, Category = "Room")
    TArray<FVector> Corners;

    FRoom()
        : Area(0.0f)
        , Perimeter(0.0f)
    {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomGenerated, const FRoom&, GeneratedRoom);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomRemoved, const FRoom&, RemovedRoom);

UCLASS(ClassGroup=(InteriorProject), meta=(BlueprintSpawnableComponent))
class INTERIORPROJECT_API URoomGeneratorComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    URoomGeneratorComponent();

    // Room Detection
    UFUNCTION(BlueprintCallable, Category = "Room Generation")
    void DetectRooms();

    UFUNCTION(BlueprintCallable, Category = "Room Generation")
    void ResetRooms();

    UFUNCTION(BlueprintPure, Category = "Room Generation")
    const TArray<FRoom>& GetDetectedRooms() const { return DetectedRooms; }

    // Room Validation
    UFUNCTION(BlueprintPure, Category = "Room Generation")
    bool IsValidRoom(const TArray<AWallActor*>& Walls) const;

    UFUNCTION(BlueprintPure, Category = "Room Generation")
    bool IsWallPartOfRoom(AWallActor* Wall) const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Room Events")
    FOnRoomGenerated OnRoomGenerated;

    UPROPERTY(BlueprintAssignable, Category = "Room Events")
    FOnRoomRemoved OnRoomRemoved;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // Room Properties
    UPROPERTY(EditAnywhere, Category = "Room Generation")
    float MinRoomArea = 100.0f;  // Minimum area for a valid room

    UPROPERTY(EditAnywhere, Category = "Room Generation")
    float WallConnectionThreshold = 1.0f;  // Maximum distance for walls to be considered connected

private:
    UPROPERTY()
    TArray<FRoom> DetectedRooms;

    bool TryFormRoom(AWallActor* StartWall, TArray<AWallActor*>& OutWalls);
    bool TryAddWallToRoom(const TArray<AWallActor*>& CurrentWalls, AWallActor* WallToAdd) const;
    bool AreWallsConnected(AWallActor* Wall1, AWallActor* Wall2) const;
    float GetConnectionAngle(AWallActor* Wall1, AWallActor* Wall2, const FVector& SharedPoint) const;
    TArray<FVector> GetRoomCorners(const TArray<AWallActor*>& Walls) const;
    float CalculateRoomArea(const TArray<FVector>& Corners) const;
    float CalculateRoomPerimeter(const TArray<AWallActor*>& Walls) const;
    void NotifyRoomGenerated(const FRoom& Room);
    void NotifyRoomRemoved(const FRoom& Room);
};