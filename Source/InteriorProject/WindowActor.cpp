#include "WindowActor.h"
#include "Components/WindowStateComponent.h"
#include "Components/WindowGeometryComponent.h"
#include "Components/WindowMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/WindowEditWidget.h"
#include "WallActor.h"

AWindowActor::AWindowActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create root component
    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = RootSceneComponent;

    // Create components
    StateComponent = CreateDefaultSubobject<UWindowStateComponent>(TEXT("StateComponent"));
    GeometryComponent = CreateDefaultSubobject<UWindowGeometryComponent>(TEXT("GeometryComponent"));
    MeshComponent = CreateDefaultSubobject<UWindowMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);

    // Create UI component
    EditWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EditWidgetComponent"));
    EditWidgetComponent->SetupAttachment(RootComponent);
    EditWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    EditWidgetComponent->SetDrawSize(FVector2D(200.0f, 100.0f));
    EditWidgetComponent->SetVisibility(false);

    // Initialize properties
    AttachedWall = nullptr;
    EditWidget = nullptr;
}

void AWindowActor::BeginPlay()
{
    Super::BeginPlay();

    // Initialize components
    MeshComponent->Initialize(GeometryComponent, StateComponent);

    // Setup input bindings
    SetupInputBindings();

    CreateEditWidget();
}

void AWindowActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    RemoveInputBindings();
    DestroyEditWidget();
    DetachFromWall();
    Super::EndPlay(EndPlayReason);
}

void AWindowActor::SetupInputBindings()
{
    if (MeshComponent)
    {
        MeshComponent->OnClicked.AddDynamic(this, &AWindowActor::OnMouseClicked);
        MeshComponent->OnBeginCursorOver.AddDynamic(this, &AWindowActor::OnMouseHoverBegin);
        MeshComponent->OnEndCursorOver.AddDynamic(this, &AWindowActor::OnMouseHoverEnd);
    }
}

void AWindowActor::RemoveInputBindings()
{
    if (MeshComponent)
    {
        MeshComponent->OnClicked.RemoveDynamic(this, &AWindowActor::OnMouseClicked);
        MeshComponent->OnBeginCursorOver.RemoveDynamic(this, &AWindowActor::OnMouseHoverBegin);
        MeshComponent->OnEndCursorOver.RemoveDynamic(this, &AWindowActor::OnMouseHoverEnd);
    }
}

void AWindowActor::OnMouseClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed)
{
    if (!IsInteractionEnabled())
        return;

    if (ButtonPressed == EKeys::LeftMouseButton)
    {
        if (StateComponent->GetState() == EWindowEditState::None)
        {
            HandleSelection();
        }
    }
}

void AWindowActor::OnMouseHoverBegin(UPrimitiveComponent* HoveredComp)
{
    if (!IsInteractionEnabled())
        return;

    if (!StateComponent->IsSelected() && StateComponent->GetState() == EWindowEditState::None)
    {
        StateComponent->SetState(EWindowEditState::Highlighted);
    }
}

void AWindowActor::OnMouseHoverEnd(UPrimitiveComponent* HoveredComp)
{
    if (StateComponent && StateComponent->GetState() == EWindowEditState::Highlighted)
    {
        StateComponent->SetState(EWindowEditState::None);
    }
}

bool AWindowActor::IsInteractionEnabled() const
{
    if (!StateComponent)
        return false;

    EWindowEditState CurrentState = StateComponent->GetState();
    return CurrentState != EWindowEditState::Placement &&
           CurrentState != EWindowEditState::Moving &&
           CurrentState != EWindowEditState::Resizing;
}

void AWindowActor::StartPlacement(const FVector& Location)
{
    SetActorLocation(Location);
    StateComponent->SetState(EWindowEditState::Placement);
    UpdateEditWidget();
}

void AWindowActor::UpdatePlacement(const FVector& Location)
{
    if (StateComponent->GetState() != EWindowEditState::Placement)
        return;

    SetActorLocation(Location);
    UpdateEditWidget();
}

void AWindowActor::FinishPlacement()
{
    if (StateComponent->GetState() != EWindowEditState::Placement)
        return;

    StateComponent->SetState(EWindowEditState::None);
    if (AttachedWall)
    {
        AttachToActor(AttachedWall, FAttachmentTransformRules::KeepWorldTransform);
    }
}

void AWindowActor::CancelPlacement()
{
    if (StateComponent->GetState() == EWindowEditState::Placement)
    {
        DetachFromWall();
        Destroy();
    }
}

FVector2D AWindowActor::GetWindowSize() const
{
    return GeometryComponent ? GeometryComponent->GetSize() : FVector2D::ZeroVector;
}

void AWindowActor::UpdateSize(const FVector2D& NewSize)
{
    if (GeometryComponent)
    {
        GeometryComponent->SetSize(NewSize);
    }
}

void AWindowActor::StartResizing()
{
    if (StateComponent)
    {
        StateComponent->SetState(EWindowEditState::Resizing);
    }
}

void AWindowActor::FinishResizing()
{
    if (StateComponent)
    {
        StateComponent->SetState(EWindowEditState::Selected);
    }
}

void AWindowActor::HandleSelection()
{
    if (StateComponent)
    {
        StateComponent->SetSelected(true);
        StateComponent->SetState(EWindowEditState::Selected);
        OnWindowSelected.Broadcast(this);
        EditWidgetComponent->SetVisibility(true);
        UpdateEditWidget();
    }
}

void AWindowActor::HandleDeselection()
{
    if (StateComponent)
    {
        StateComponent->SetSelected(false);
        StateComponent->SetState(EWindowEditState::None);
        EditWidgetComponent->SetVisibility(false);
    }
}

bool AWindowActor::IsSelected() const
{
    return StateComponent ? StateComponent->IsSelected() : false;
}

void AWindowActor::AttachToWall(AWallActor* Wall, EWallSide Side)
{
    if (Wall != AttachedWall)
    {
        DetachFromWall();
        AttachedWall = Wall;
        GeometryComponent->SetWallSide(Side);
    }
}

void AWindowActor::DetachFromWall()
{
    if (AttachedWall)
    {
        DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        AttachedWall = nullptr;
        GeometryComponent->SetWallSide(EWallSide::None);
    }
}

void AWindowActor::CreateEditWidget()
{
    if (EditWidgetComponent && !EditWidget)
    {
        EditWidget = CreateWidget<UWindowEditWidget>(GetWorld(), EditWidgetComponent->GetWidgetClass());
        if (EditWidget)
        {
            EditWidget->SetTargetWindow(this);
            EditWidgetComponent->SetWidget(EditWidget);
        }
    }
}

void AWindowActor::DestroyEditWidget()
{
    if (EditWidget)
    {
        EditWidget->RemoveFromParent();
        EditWidget = nullptr;
    }
}

void AWindowActor::UpdateEditWidget()
{
    if (!EditWidget || !EditWidgetComponent)
        return;

    // Update widget position and size based on window state
    FVector WidgetLocation = GetActorLocation();
    WidgetLocation.Z += 50.0f; // Offset above window
    EditWidgetComponent->SetWorldLocation(WidgetLocation);

    // Update widget content
    EditWidget->UpdateSizeText();
}