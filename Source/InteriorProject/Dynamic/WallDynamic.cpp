#include "WallDynamic.h"
#include "PlaceableActor.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "GeometryScript/MeshUVFunctions.h"
#include "GeometryScript/MeshBooleanFunctions.h"
#include "GeometryScript/MeshSubdivideFunctions.h"
#include "GeometryScript/MeshAssetFunctions.h"
#include "InteriorProject/GUI/GUIWall.h"

AWallDynamic::AWallDynamic()
{
	PrimaryActorTick.bCanEverTick = false;
	DynamicMeshComponent->bEnableComplexCollision = true;
	DynamicMeshComponent->CollisionType = CTF_UseComplexAsSimple;
}

void AWallDynamic::GenerateWall(UDynamicMesh* DynamicMesh)
{
	FTransform Transform;
	Transform.SetTranslation(FVector3d(0, WallWidth/2, 0));
	
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendBox(
		DynamicMesh,
		FGeometryScriptPrimitiveOptions(),
		Transform, 
		WallThickness,
		WallWidth,
		WallHeight,
		WallThickness/100,
		WallWidth/100,
		WallThickness/100
	);
    
		UGeometryScriptLibrary_MeshUVFunctions::SetMeshUVsFromBoxProjection(
		DynamicMesh,
		0,
		FTransform::Identity.GetScaled(100),
		FGeometryScriptMeshSelection(),
		2
	);
	
}

void AWallDynamic::AddHoleGeometry(APlaceableActor* PlaceableActor)
{
	// Duvarın dynamic mesh'i
	UDynamicMesh* WallDynamicMesh = DynamicMeshComponent->GetDynamicMesh();

	// PlaceableActor'ün static mesh'ini dynamic mesh'e çevirelim
	UDynamicMesh* PlaceableDynamicMesh = NewObject<UDynamicMesh>();
	EGeometryScriptOutcomePins Outcome;
    
	UGeometryScriptLibrary_StaticMeshFunctions::CopyMeshFromStaticMesh(
		PlaceableActor->GetSimpleMesh(),
		PlaceableDynamicMesh,
		FGeometryScriptCopyMeshFromAssetOptions(),
		FGeometryScriptMeshReadLOD(),
		Outcome
	);
    
	if (Outcome == EGeometryScriptOutcomePins::Success)
	{
		FTransform PlaceableTransform = PlaceableActor->GetReleativeTransform();
		
		FVector NewScale(WallThickness / PlaceableActor->GetSimpleMesh()->GetBounds().BoxExtent.X, 1.0f, 1.0f);
		PlaceableTransform.SetScale3D(NewScale);

		UGeometryScriptLibrary_MeshBooleanFunctions::ApplyMeshBoolean(
			WallDynamicMesh,
			FTransform::Identity,
			PlaceableDynamicMesh,
			PlaceableTransform,  // Ölçeklenmiş transform'u kullan
			EGeometryScriptBooleanOperation::Subtract,
			FGeometryScriptMeshBooleanOptions()
		);
	}
}

void AWallDynamic::UpdateDimensions(float NewThickness, float NewWidth, float NewHeight)
{
	WallThickness = NewThickness;
	WallWidth = NewWidth;
	WallHeight = NewHeight;
	
	UpdateWall();
}

void AWallDynamic::RemoveHole(APlaceableActor* PlaceableDynamic)
{
	Placeables.Remove(PlaceableDynamic);

	UpdateWall();
}

void AWallDynamic::UpdateWall()
{
	UDynamicMesh* DynamicMesh = DynamicMeshComponent->GetDynamicMesh();
	DynamicMesh->Reset();
    
	GenerateWall(DynamicMesh);

	// Add holes
	for(auto& PlaceableDynamic : Placeables)
	{
		AddHoleGeometry(PlaceableDynamic);
	}
}

void AWallDynamic::SetSelectionState(bool bSelected)
{
	DynamicMeshComponent->SetMaterial(0, bSelected ? SelectedMaterial : NormalMaterial);
	if(bSelected)
	{
		for (auto Placeable : Placeables)
		{
			Placeable->GetGUIPlaceable()->SetSelectionState(false);
		}
	}
}

void AWallDynamic::BeginPlay()
{
	Super::BeginPlay();

	DynamicMeshComponent->OnClicked.AddDynamic(this, &AWallDynamic::ClickedComponent);
}

void AWallDynamic::ClickedComponent(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	GUIWall->SetSelectionState(true);
}
