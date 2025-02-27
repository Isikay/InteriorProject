// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/Structs.h"
#include "WallConnection.generated.h"

class AFloorActor;
class UWidgetComponent;

UCLASS()
class INTERIORPROJECT_API AWallConnection : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWallConnection();
	
	void SetWallManager(AFloorActor* Manager);

	void SetConnectedWalls(const TArray<FWallCorner>& Walls);

	FORCEINLINE TArray<FWallCorner> GetConnectedWalls() const { return ConnectedWalls; }

	void HandlePressed();
	void HandleHovered();
	void HandleUnhovered();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|UI")
	UWidgetComponent* ConnectionWidgetComponent;
	
private:

	UPROPERTY()
	AFloorActor* OwningWallManager;

	TArray<FWallCorner> ConnectedWalls;
};
