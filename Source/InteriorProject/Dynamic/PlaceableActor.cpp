// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaceableActor.h"
#include "WallDynamic.h"
#include "Components/ArrowComponent.h"
#include "InteriorProject/Base/IPPlayerController.h"


// Sets default values
APlaceableActor::APlaceableActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	SimpleMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SimplyMeshComponent"));
	SimpleMeshComponent->SetupAttachment(MeshComponent);
	SimpleMeshComponent->SetHiddenInGame(true);
	
	/*LeftArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftArrow"));
	LeftArrow->SetupAttachment(MeshComponent);
	LeftArrow->SetRelativeRotation(FRotator(0, -90, 0));

	RightArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightArrow"));
	RightArrow->SetupAttachment(MeshComponent);
	RightArrow->SetRelativeRotation(FRotator(0, 90, 0));

	UpArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	UpArrow->SetupAttachment(MeshComponent);
	UpArrow->SetRelativeRotation(FRotator(90, 0, 0));

	DownArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DownArrow"));
	DownArrow->SetupAttachment(MeshComponent);
	DownArrow->SetRelativeRotation(FRotator(-90, 0, 0));*/
}

// Called when the game starts or when spawned
void APlaceableActor::BeginPlay()
{
	Super::BeginPlay();

	PlayerController =  Cast<AIPPlayerController>(GetWorld()->GetFirstPlayerController());
	
	//DynamicMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &APlaceableDynamic::ComponentBeginOverlap);
	SimpleMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &APlaceableActor::ComponentBeginOverlap);
	SimpleMeshComponent->OnComponentEndOverlap.AddDynamic(this, &APlaceableActor::ComponentEndOverlap);
	SimpleMeshComponent->OnClicked.AddDynamic(this, &APlaceableActor::ComponentClicked);
	SimpleMeshComponent->OnReleased.AddDynamic(this, &APlaceableActor::ComponentReleased);
	
	//SetArrowsHide(false);
}

void APlaceableActor::AttachToWall(AWallDynamic* WallToAttach, float Position)
{
	if(WallToAttach)
	{
		//OwnerWall = WallToAttach;
		FAttachmentTransformRules AttachmentRules= FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
		AttachmentRules.LocationRule = EAttachmentRule::SnapToTarget;
		AttachToActor(WallToAttach, AttachmentRules);
		SetActorRelativeLocation(FVector(0, Position, bIsWindow ? WallToAttach->GetWallHeight()/2 : 0));
		WallToAttach->AddPlaceable(this);
	}
	else
	{
		if(OwnerWall)
		{
			DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			OwnerWall->RemovePlaceable(this);
			//OwnerWall = nullptr;
		}
	}
}

void APlaceableActor::HandleDeselection()
{
	if(GUIPlaceable)
	{
		GUIPlaceable->SetSelectionState(false);
	}
}

void APlaceableActor::SetSelected(bool bSelected)
{
	SimpleMeshComponent->SetHiddenInGame(!bSelected);
	MeshComponent->SetHiddenInGame(bSelected);
}

const FTransform APlaceableActor::GetReleativeTransform()
{
	FTransform Transform = MeshComponent->GetRelativeTransform();
	Transform.SetLocation(Transform.GetLocation()+SimpleMeshComponent->GetRelativeLocation());
	return Transform;
}


void APlaceableActor::SetArrowsHide(bool bVisible)
{
	LeftArrow->SetHiddenInGame(bVisible);
	RightArrow->SetHiddenInGame(bVisible);
	UpArrow->SetHiddenInGame(bVisible);
	DownArrow->SetHiddenInGame(bVisible);
}

void APlaceableActor::SetupArrowsPosition()
{
    /*AWallDynamic* ParentWall = Cast<AWallDynamic>(GetAttachParentActor());
    if (!ParentWall || !DynamicMeshComponent) return;

    const FVector MeshSize = DynamicMeshComponent->Bounds.GetBox().GetSize();
    const FVector WallLocation = ParentWall->GetActorLocation();
    const FQuat WallRotation = ParentWall->GetActorQuat() * FQuat(FRotator(0, 90, 0));
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(ParentWall);
    
    const FVector ActorLocation = GetActorLocation();
    const FVector RelativeLocation = bIsWindow ? FVector::ZeroVector : FVector(0, 0, MeshSize.Z/2);
    const FVector MeshCenter = ActorLocation + RelativeLocation;

    // Setup arrow positions and perform traces
    const FVector DownRelative = FVector(0, 0, -MeshSize.Z/2) + RelativeLocation;
    const FVector UpRelative = FVector(0, 0, MeshSize.Z/2) + RelativeLocation;
    const FVector RightRelative = FVector(0, MeshSize.Y/2, 0) + RelativeLocation;
    const FVector LeftRelative = FVector(0, -MeshSize.Y/2, 0) + RelativeLocation;

    // Right trace
    TraceAndSetupArrow(
        ActorLocation + RightRelative,
        WallLocation + ParentWall->GetActorRotation().RotateVector(FVector(0, ParentWall->GetWallWidth(), 0)) + FVector(0, 0, MeshCenter.Z),
        RightArrow,
        RightRelative,
        WallRotation,
        MeshSize/2,
        QueryParams,
        FColor::Red
    );

    // Left trace
    TraceAndSetupArrow(
        ActorLocation + LeftRelative,
        WallLocation + FVector(0, 0, MeshCenter.Z),
        LeftArrow,
        LeftRelative,
        WallRotation,
        MeshSize/2,
        QueryParams,
        FColor::Red
    );

    // Up trace
    TraceAndSetupArrow(
        ActorLocation + UpRelative,
        FVector(ActorLocation.X, ActorLocation.Y, ParentWall->GetWallHeight()),
        UpArrow,
        UpRelative,
        WallRotation,
        MeshSize/2,
        QueryParams,
        FColor::Blue
    );

    // Down trace
    TraceAndSetupArrow(
        ActorLocation + DownRelative,
        FVector(ActorLocation.X, ActorLocation.Y, 0),
        DownArrow,
        DownRelative,
        WallRotation,
        MeshSize/2,
        QueryParams,
        FColor::Blue
    );*/
}

void APlaceableActor::Place(AWallDynamic* Wall)
{
	// If the placeable actor is already placed on the wall, move it to the new position
	if(OwnerWall == Wall)
	{
		OwnerWall->UpdateWall();
	}
	else
	{
		if(OwnerWall)
		{
			UE_LOG( LogTemp, Warning, TEXT("OwnerWall var ve eski duvar ve update oluyor") );
			OwnerWall->UpdateWall();
		}
		OwnerWall = Wall;
		OwnerWall->AddHoleGeometry(this);
	}
}

void APlaceableActor::UpdatePositionY(float Position)
{
	if(OwnerWall)
	{
		FVector ReleativeLocation = MeshComponent->GetRelativeLocation();
		SetActorRelativeLocation(FVector(ReleativeLocation.X, Position, ReleativeLocation.Z));
	}
}

void APlaceableActor::TraceAndSetupArrow(const FVector& Start, const FVector& End, UArrowComponent* Arrow, const FVector& ArrowRelativeLocation, const FQuat& WallRotation, const FVector& BoxExtent, const FCollisionQueryParams& QueryParams, const FColor& DebugColor)
{
	FHitResult HitResult;
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		WallRotation,
		ECC_Visibility,
		FCollisionShape::MakeBox(BoxExtent),
		QueryParams
	);

	float Distance = bHit ? (HitResult.Location - Start).Size() : (End - Start).Size();
    
	Arrow->SetRelativeLocation(ArrowRelativeLocation);
	Arrow->SetArrowLength(Distance);
	Arrow->SetArrowColor(FLinearColor::Red);

	UE_LOG(LogTemp, Warning, TEXT("Arrow Length: %f"), Distance);

#if WITH_EDITOR
	DrawDebugBox(GetWorld(), Start, BoxExtent, WallRotation, DebugColor, false, 25.0f);
	DrawDebugBox(GetWorld(), bHit ? HitResult.Location : End, BoxExtent, WallRotation, DebugColor, false, 25.0f);
	DrawDebugLine(GetWorld(), Start, bHit ? HitResult.Location : End, DebugColor, false, 25.0f);
#endif
}

void APlaceableActor::ComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapped actor is a placeable actor
	if(APlaceableActor* OverlappedPlaceable = Cast<APlaceableActor>(OtherActor))
	{
		OverlappedPlaceables.Add(OverlappedPlaceable);
		// Send feed back to GUIPlaceable
		if(GUIPlaceable)
			GUIPlaceable->SetPlacementValid(false);
	}
}

void APlaceableActor::ComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	// Check if the overlapped actor is a placeable actor
	if(APlaceableActor* OverlappedPlaceable = Cast<APlaceableActor>(OtherActor))
	{
		OverlappedPlaceables.Remove(OverlappedPlaceable);
		if(OverlappedPlaceables.IsEmpty())
		{
			// Send feed back to GUIPlaceable
			if(GUIPlaceable)
				GUIPlaceable->SetPlacementValid(true);
		}
	}
}

void APlaceableActor::ComponentClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (ButtonPressed == EKeys::LeftMouseButton && OwnerWall)
	{
		if(GUIPlaceable->GetSelectedState())
		{
			SimpleMeshComponent->SetWorldScale3D(SimpleMeshComponent->GetRelativeScale3D() + FVector(0.3f, 0, 0));
			// Start a timer that checks mouse position frequently
			GetWorld()->GetTimerManager().SetTimer(
				DraggingTimerHandle,
				this,
				&APlaceableActor::UpdatePositionUnderMouse,
				0.05,  // Very small interval for smooth movement
				true   // Looping
			);
		}
		else
		{
			GUIPlaceable->SetSelectionState(true);
		}
	}
}

void APlaceableActor::ComponentReleased(UPrimitiveComponent* TouchedComponent, FKey ButtonReleased)
{
	UE_LOG( LogTemp, Warning, TEXT("ComponentReleased") );
	if (ButtonReleased == EKeys::LeftMouseButton)
	{
		SimpleMeshComponent->SetWorldScale3D(SimpleMeshComponent->GetRelativeScale3D() - FVector(0.3f, 0, 0));
		// Stop the timer
		GetWorld()->GetTimerManager().ClearTimer(DraggingTimerHandle);
		GUIPlaceable->FinalizeWallPlacement();
		if(OwnerWall)
			OwnerWall->UpdateWall();
	}
}

void APlaceableActor::UpdatePositionUnderMouse()
{
	if (!OwnerWall || !PlayerController) return;

	FHitResult HitResult;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes = {UEngineTypes::ConvertToObjectType(ECC_WorldStatic)};
    
	if (PlayerController->GetHitResultUnderCursorForObjects(ObjectTypes, true, HitResult))
	{
		if (AWallDynamic* HitWall = Cast<AWallDynamic>(HitResult.GetActor()))
		{
			// Convert world hit location to wall's local space
			FVector LocalHitLocation = HitWall->GetActorTransform().InverseTransformPosition(HitResult.Location);
			FVector CurrentRelative = MeshComponent->GetRelativeLocation();
			// Update position while maintaining X
			CurrentRelative.Y = LocalHitLocation.Y;
			CurrentRelative.Z = LocalHitLocation.Z - SimpleMeshComponent->GetRelativeLocation().Z;
            
			if(!IsAttachedTo(HitWall))
			{
				GUIPlaceable->DetachFromWall();
                
				GUIPlaceable->AttachToWall(HitWall->GetGUIWall(), LocalHitLocation.Y);
			}
			else
			{
				SetActorRelativeLocation(CurrentRelative);
				
				if (GUIPlaceable)
				{
					GUIPlaceable->UpdateWallPosition(LocalHitLocation.Y);
				}
			}
		}
	}
}