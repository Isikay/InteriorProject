// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WallDynamic.h"
#include "InteriorProject/GUI/GUIPlaceable.h"
#include "PlaceableActor.generated.h"

class UArrowComponent;
class AIPPlayerController;

UCLASS()
class INTERIORPROJECT_API APlaceableActor : public AActor
{
	GENERATED_BODY()

public:
	
	// Sets default values for this actor's properties
	APlaceableActor();

	FORCEINLINE bool IsWindow() const { return bIsWindow; }
	
	void SetupArrowsPosition();

	void Place(AWallDynamic* Wall);
	
	void UpdatePositionY(float Position);
	
	void AttachToWall(AWallDynamic* WallToAttach, float Position = 0.0f);

	UFUNCTION()
	void HandleDeselection();
	
	void SetSelected(bool bSelected);

	FORCEINLINE void SetOwnerPlaceable(UGUIPlaceable* Placeable) { GUIPlaceable = Placeable; }

	FORCEINLINE UStaticMesh* GetSimpleMesh() const { return SimpleMeshComponent->GetStaticMesh();}

	FORCEINLINE UGUIPlaceable* GetGUIPlaceable() const { return GUIPlaceable; }

	const FTransform GetReleativeTransform();


protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void SetArrowsHide(bool bVisible);

	void TraceAndSetupArrow(const FVector& Start, const FVector& End, UArrowComponent* Arrow, const FVector& ArrowRelativeLocation, const FQuat& WallRotation, const FVector& BoxExtent, const FCollisionQueryParams& QueryParams, const FColor& DebugColor);

	UFUNCTION()
	void ComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void ComponentClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UFUNCTION()
	void ComponentReleased(UPrimitiveComponent* TouchedComponent, FKey ButtonReleased);

	void UpdatePositionUnderMouse();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SimpleMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UArrowComponent* LeftArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UArrowComponent* RightArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UArrowComponent* UpArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UArrowComponent* DownArrow;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	bool bIsWindow = false;

	UPROPERTY()
	AWallDynamic* OwnerWall;

	UPROPERTY()
	UGUIPlaceable* GUIPlaceable;

	UPROPERTY()
	TArray<APlaceableActor*> OverlappedPlaceables;

	UPROPERTY()
	AIPPlayerController* PlayerController;

	FTimerHandle DraggingTimerHandle;
};
