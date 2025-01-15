#include "WindowMeshComponent.h"
#include "WindowGeometryComponent.h"
#include "WindowStateComponent.h"

UWindowMeshComponent::UWindowMeshComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    // Set collision defaults
    SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetGenerateOverlapEvents(true);

    GeometryComponent = nullptr;
    StateComponent = nullptr;
}

void UWindowMeshComponent::BeginPlay()
{
    Super::BeginPlay();

    // Store original bounds for scaling calculations
    if (GetStaticMesh())
    {
        OriginalMeshBounds = GetStaticMesh()->GetBoundingBox().GetSize();
    }
}

void UWindowMeshComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CleanupComponents();
    Super::EndPlay(EndPlayReason);
}

void UWindowMeshComponent::Initialize(UWindowGeometryComponent* Geometry, UWindowStateComponent* State)
{
    CleanupComponents();

    GeometryComponent = Geometry;
    StateComponent = State;

    if (GeometryComponent)
    {
        GeometryComponent->OnGeometryChanged.AddUObject(this, &UWindowMeshComponent::OnGeometryChanged);
    }

    if (StateComponent)
    {
        StateComponent->OnWindowStateChanged.AddUObject(this, &UWindowMeshComponent::OnStateChanged);
        StateComponent->OnWindowSelectionChanged.AddUObject(this, &UWindowMeshComponent::OnSelectionChanged);
    }

    // Initial updates
    UpdateMeshScale();
    UpdateMaterial();
}

void UWindowMeshComponent::OnGeometryChanged()
{
    UpdateMeshScale();
}

void UWindowMeshComponent::OnStateChanged(EWindowEditState NewState, EWindowEditState OldState)
{
    UpdateMaterial();
}

void UWindowMeshComponent::OnSelectionChanged(bool bSelected)
{
    UpdateMaterial();
}

void UWindowMeshComponent::UpdateMeshScale()
{
    if (!GeometryComponent || OriginalMeshBounds.IsZero())
        return;

    const FVector2D& WindowSize = GeometryComponent->GetSize();
    
    // Calculate scale factors to achieve desired size
    float ScaleX = WindowSize.X / OriginalMeshBounds.X;
    float ScaleY = GeometryComponent->WindowDepth / OriginalMeshBounds.Y;
    float ScaleZ = WindowSize.Y / OriginalMeshBounds.Z;

    // Apply the new scale
    SetRelativeScale3D(FVector(ScaleX, ScaleY, ScaleZ));
}

void UWindowMeshComponent::UpdateMaterial()
{
    if (!StateComponent)
        return;

    UMaterialInterface* MaterialToUse = nullptr;

    switch (StateComponent->GetState())
    {
        case EWindowEditState::Placement:
            MaterialToUse = PlacementMaterial;
            break;
        case EWindowEditState::Selected:
        case EWindowEditState::Moving:
        case EWindowEditState::Resizing:
            MaterialToUse = SelectedMaterial;
            break;
        case EWindowEditState::Highlighted:
            MaterialToUse = HighlightMaterial;
            break;
        default:
            MaterialToUse = DefaultMaterial;
            break;
    }

    if (MaterialToUse)
    {
        SetMaterial(0, MaterialToUse);
    }
}

void UWindowMeshComponent::CleanupComponents()
{
    if (GeometryComponent)
    {
        GeometryComponent->OnGeometryChanged.RemoveAll(this);
        GeometryComponent = nullptr;
    }

    if (StateComponent)
    {
        StateComponent->OnWindowStateChanged.RemoveAll(this);
        StateComponent->OnWindowSelectionChanged.RemoveAll(this);
        StateComponent = nullptr;
    }
}