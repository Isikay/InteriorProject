// RoomManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/RoomGeneratorComponent.h"  // Include to get FRoom struct
#include "RoomManager.generated.h"

class URoomGeneratorComponent;
class UProceduralMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomCreated, const FRoom&, NewRoom);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomDeleted, const FRoom&, DeletedRoom);

UCLASS()
class INTERIORPROJECT_API ARoomManager : public AActor
{
    GENERATED_BODY()

public:
    ARoomManager();

    // Room Operations
    UFUNCTION(BlueprintCallable, Category = "Room Management")
    void UpdateRooms();
    
    UFUNCTION(BlueprintCallable, Category = "Room Management")
    void ClearRooms();
    
    UFUNCTION(BlueprintCallable, Category = "Room Management")
    void ToggleRoomVisualization();

    // Room Queries
    UFUNCTION(BlueprintPure, Category = "Room Management")
    TArray<FRoom> GetAllRooms() const;
    
    UFUNCTION(BlueprintCallable, Category = "Room Management")
    bool GetRoomAtLocation(const FVector& Location, FRoom& OutRoom);
    
    UFUNCTION(BlueprintPure, Category = "Room Management")
    bool IsLocationInRoom(const FVector& Location, const FRoom& Room) const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Room Events")
    FOnRoomCreated OnRoomCreated;
    
    UPROPERTY(BlueprintAssignable, Category = "Room Events")
    FOnRoomDeleted OnRoomDeleted;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    URoomGeneratorComponent* RoomGenerator;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProceduralMeshComponent* FloorMeshComponent;

    // Room settings
    UPROPERTY(EditDefaultsOnly, Category = "Room Visualization")
    UMaterialInterface* FloorMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "Room Visualization")
    float FloorHeight = 0.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Room Visualization")
    bool bShowRoomVisualization = true;

private:
    // Event handlers
    UFUNCTION()
    void OnRoomGenerated(const FRoom& GeneratedRoom);
    
    UFUNCTION()
    void OnRoomRemoved(const FRoom& RemovedRoom);

    // Helper functions
    void CreateFloorMesh(const FRoom& Room);
    void ClearFloorMeshes();
    void UpdateRoomVisualization();
    FString GetRoomInfo(const FRoom& Room) const;

    // Room storage
    UPROPERTY()
    TArray<FRoom> CurrentRooms;

    bool bVisualizationEnabled;
};