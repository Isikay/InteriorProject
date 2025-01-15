#include "WallMeshComponent.h"
#include "WallGeometryComponent.h"
#include "WallStateComponent.h"
#include "WallWindowComponent.h"
#include "InteriorProject/Utils/WallMeshGenerator.h"

UWallMeshComponent::UWallMeshComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bUseAsyncCooking = true;
    GeometryComponent = nullptr;
    StateComponent = nullptr;
    WindowComponent = nullptr;
}

void UWallMeshComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UWallMeshComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CleanupComponents();
    Super::EndPlay(EndPlayReason);
}

void UWallMeshComponent::Initialize(UWallGeometryComponent* Geometry, UWallStateComponent* State, UWallWindowComponent* Windows)
{
    CleanupComponents();

    GeometryComponent = Geometry;
    StateComponent = State;
    WindowComponent = Windows;

    // Setup event bindings
    if (GeometryComponent)
    {
        GeometryComponent->OnGeometryChanged.AddUObject(this, &UWallMeshComponent::OnGeometryChanged);
    }

    if (StateComponent)
    {
        StateComponent->OnWallStateChanged.AddUObject(this, &UWallMeshComponent::OnStateChanged);
        StateComponent->OnWallSelectionChanged.AddUObject(this, &UWallMeshComponent::OnSelectionChanged);
    }

    if (WindowComponent)
    {
        WindowComponent->OnWindowsModified.AddUObject(this, &UWallMeshComponent::OnWindowsModified);
    }

    UpdateMeshGeometry();
    UpdateMaterials();
}

void UWallMeshComponent::UpdateMeshGeometry()
{
    if (!GeometryComponent)
        return;

    // Get wall geometry data in world space
    FVector Start = GeometryComponent->GetStart();
    FVector End = GeometryComponent->GetEnd();
    float Height = GeometryComponent->WallHeight;
    float Thickness = GeometryComponent->WallThickness;

    // Get windows if we have any
    TArray<FWallWindow> Windows;
    if (WindowComponent)
    {
        Windows = WindowComponent->GetWindows();
    }

    // Convert to local space
    FVector ActorLocation = GetOwner()->GetActorLocation();
    FVector LocalStart = Start - ActorLocation;
    FVector LocalEnd = End - ActorLocation;

    // Generate mesh data using WallMeshGenerator
    FWallMeshData MeshData = UWallMeshGenerator::GenerateWallMesh(
        LocalStart, LocalEnd, Height, Thickness, Windows);

    // Create mesh section
    CreateMeshSection(0, MeshData.Vertices, MeshData.Triangles, 
                     MeshData.Normals, MeshData.UVs, 
                     TArray<FColor>(), MeshData.Tangents, true);

    // Update collision
    if (bUseComplexAsSimpleCollision)
    {
        AddCollisionConvexMesh(MeshData.Vertices);
    }
}

void UWallMeshComponent::UpdateMaterials()
{
    if (!StateComponent)
        return;

    UMaterialInterface* MaterialToUse = nullptr;

    switch (StateComponent->GetState())
    {
        case EWallState::Drawing:
            MaterialToUse = DrawingMaterial;
            break;
        case EWallState::Selected:
            MaterialToUse = SelectionMaterial;
            break;
        case EWallState::Highlighted:
            MaterialToUse = HighlightMaterial;
            break;
        default:
            MaterialToUse = DefaultMaterial;
            break;
    }

    if (MaterialToUse)
    {
        UpdateWallMaterial(MaterialToUse);
    }
}

void UWallMeshComponent::UpdateWallMaterial(UMaterialInterface* Material)
{
    if (Material)
    {
        SetMaterial(0, Material);
    }
}

void UWallMeshComponent::OnGeometryChanged()
{
    UpdateMeshGeometry();
}

void UWallMeshComponent::OnStateChanged(EWallState NewState, EWallState OldState)
{
    UpdateMeshGeometry();
    UpdateMaterials();
}

void UWallMeshComponent::OnSelectionChanged(bool bSelected)
{
    UpdateMaterials();
}

void UWallMeshComponent::OnWindowsModified()
{
    UpdateMeshGeometry();
}

void UWallMeshComponent::CleanupComponents()
{
    if (GeometryComponent)
    {
        GeometryComponent->OnGeometryChanged.RemoveAll(this);
        GeometryComponent = nullptr;
    }

    if (StateComponent)
    {
        StateComponent->OnWallStateChanged.RemoveAll(this);
        StateComponent->OnWallSelectionChanged.RemoveAll(this);
        StateComponent = nullptr;
    }

    if (WindowComponent)
    {
        WindowComponent->OnWindowsModified.RemoveAll(this);
        WindowComponent = nullptr;
    }
}