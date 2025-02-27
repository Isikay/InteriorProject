// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/InteriorTypes.h"
#include "Structs/Structs.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DragInterface.h"
#include "FloorActor.generated.h"

class AIPDrawingModePawn;
class AWallActor;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWallDrawingStarted, AWallActor* /*Wall*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWallDrawingEnded, AWallActor* /*Wall*/);

UCLASS()
class INTERIORPROJECT_API AFloorActor : public AActor, public IDragInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFloorActor();

	// IDragInterface
	virtual void Dragg(const FVector& DraggedDistance) override;

	FORCEINLINE void SetDrawingModePawn(AIPDrawingModePawn* Pawn) { DrawingModePawn = Pawn; }	
	
	// Wall Creation
	void StartOrEndWallDrawing(const FVector& Location);
	void EndWallDrawing();
	void CancelWallDrawing();
	
	// Wall Movement
	void StartWallMove(AWallActor* Wall);
	void UpdateWallMove(const FVector& NewLocation);
	void EndWallMove();
	
	// Wall Queries
	TArray<AWallActor*> GetAllWalls() const;
	TArray<AWallActor*> GetNearbyWalls(const FVector& Location, float Radius) const;
	TArray<FVector> GetWallSnapPoints() const;
	TArray<AWallActor*> GetConnectedWalls(AWallActor* Wall) const;
    
	// Getters
	FORCEINLINE AWallActor* GetCurrentWall() const { return CurrentWall; }
	FORCEINLINE AWallActor* GetSelectedWall() const { return SelectedWall; }
	
	UFUNCTION()
	void SetEditMode(EEditMode NewMode, EEditMode OldMode);

	// Events
	FOnWallDrawingStarted OnWallDrawingStarted;
	FOnWallDrawingEnded OnWallDrawingEnded;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Core Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<AWallActor> WallActorClass;

	// Properties
	UPROPERTY(EditDefaultsOnly, Category = "Wall")
	float WallConnectionThreshold = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Wall")
	float WallSearchRadius = 500.0f;

private:
	
	UPROPERTY()
	AWallActor* CurrentWall;
    
	UPROPERTY()
	AWallActor* SelectedWall;

	UPROPERTY()
	AWallActor* MovingWall;
    
	UPROPERTY()
	TArray<AWallActor*> ConnectedWalls;

	TArray<FVector> InitialPositions;
	FVector MoveStartLocation;
    
	// Internal event handlers
	void HandleWallDrawingStarted(AWallActor* Wall);
	void HandleWallDrawingEnded(AWallActor* Wall);
    
	// Helper functions
	void CleanupWalls();
	void UpdateConnectedWallPositions(const FVector& DeltaMove);
	bool AreWallsConnected(AWallActor* Wall1, AWallActor* Wall2) const;
	float GetConnectionAngle(AWallActor* Wall1, AWallActor* Wall2, const FVector& SharedPoint) const;
    
	UFUNCTION()
	void OnWallDestroyed(AActor* DestroyedActor);
	
	UFUNCTION()
	void OnCornerHovered(AWallActor* Wall, bool bIsStartCorner, bool bIsHovered);

	UFUNCTION()
	void OnCornerClicked(AWallActor* Wall, bool bIsStartCorner);
	
	UFUNCTION()
	void OnFloorClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed);

	FWallCorner HoveredCorner;

	UPROPERTY()
	AIPDrawingModePawn* DrawingModePawn;
};
