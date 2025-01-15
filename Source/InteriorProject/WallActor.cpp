#include "WallActor.h"
#include "Components/WallGeometryComponent.h"
#include "Components/WallStateComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/CornerResizeWidget.h"
#include "UI/WallMeasurementWidget.h"
#include "Base/IPDrawingModePawn.h"
#include "Components/WallMeshComponent.h"
#include "Components/WallInteractionComponent.h"
#include "Components/WallWindowComponent.h"

AWallActor::AWallActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // Create core components
    GeometryComponent = CreateDefaultSubobject<UWallGeometryComponent>(TEXT("GeometryComponent"));
    GeometryComponent->SetupAttachment(RootComponent);

    StateComponent = CreateDefaultSubobject<UWallStateComponent>(TEXT("StateComponent"));
    
    MeshComponent = CreateDefaultSubobject<UWallMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);

    InteractionComponent = CreateDefaultSubobject<UWallInteractionComponent>(TEXT("InteractionComponent"));
    
    WindowComponent = CreateDefaultSubobject<UWallWindowComponent>(TEXT("WindowComponent"));

    // Create UI components
    StartCornerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("StartCornerWidget"));
    StartCornerWidget->SetupAttachment(RootComponent);
    StartCornerWidget->SetWidgetSpace(EWidgetSpace::Screen);
    StartCornerWidget->SetDrawSize(FVector2D(32.0f, 32.0f));
    StartCornerWidget->SetVisibility(false);

    EndCornerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("EndCornerWidget"));
    EndCornerWidget->SetupAttachment(RootComponent);
    EndCornerWidget->SetWidgetSpace(EWidgetSpace::Screen);
    EndCornerWidget->SetDrawSize(FVector2D(32.0f, 32.0f));
    EndCornerWidget->SetVisibility(false);

    MeasurementWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MeasurementWidget"));
    MeasurementWidget->SetupAttachment(RootComponent);
    MeasurementWidget->SetWidgetSpace(EWidgetSpace::Screen);
    MeasurementWidget->SetDrawSize(FVector2D(200.0f, 50.0f));
    MeasurementWidget->SetVisibility(false);
}

void AWallActor::BeginPlay()
{
    Super::BeginPlay();

    // Initialize components
    InteractionComponent->Initialize(this);
    WindowComponent->Initialize(this);
    MeshComponent->Initialize(GeometryComponent, StateComponent, WindowComponent);

    // Setup event bindings
    StateComponent->OnWallStateChanged.AddUObject(this, &AWallActor::OnWallStateChanged);
    StateComponent->OnWallSelectionChanged.AddUObject(this, &AWallActor::OnSelectionChanged);
    // Setup interaction bindings
    if (InteractionComponent)
    {
        InteractionComponent->OnWallClicked.AddDynamic(this, &AWallActor::OnWallClicked);
        InteractionComponent->OnWallHoverBegin.AddDynamic(this, &AWallActor::OnWallHoverBegin);
        InteractionComponent->OnWallHoverEnd.AddDynamic(this, &AWallActor::OnWallHoverEnd);
    }
    WindowComponent->OnWindowsModified.AddUObject(this, &AWallActor::OnWindowsModified);
    GeometryComponent->OnGeometryChanged.AddUObject(this, &AWallActor::OnGeometryChanged);

    // Create and setup widgets
    CreateWidgets();
}

void AWallActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (OwningDrawingPawn)
    {
        UnsubscribeFromEditModeChanges(OwningDrawingPawn);
    }
    // Cleanup interaction bindings
    if (InteractionComponent)
    {
        InteractionComponent->OnWallClicked.RemoveDynamic(this, &AWallActor::OnWallClicked);
        InteractionComponent->OnWallHoverBegin.RemoveDynamic(this, &AWallActor::OnWallHoverBegin);
        InteractionComponent->OnWallHoverEnd.RemoveDynamic(this, &AWallActor::OnWallHoverEnd);
    }

    Super::EndPlay(EndPlayReason);
}

void AWallActor::StartDrawing(const FVector& StartPoint)
{
    GeometryComponent->UpdateStartPoint(StartPoint);
    GeometryComponent->UpdateEndPoint(StartPoint);
    StateComponent->SetState(EWallState::Drawing);
    UpdateWidgetVisibility();
}

void AWallActor::UpdateDrawing(const FVector& CurrentPoint)
{
    if (StateComponent->GetState() == EWallState::Drawing)
    {
        GeometryComponent->UpdateEndPoint(CurrentPoint);
        UpdateWidgets();
    }
}

void AWallActor::EndDrawing()
{
    if (StateComponent->GetState() != EWallState::Drawing)
        return;

    if (GeometryComponent->GetLength() > GeometryComponent->MinWallLength)
    {
        StateComponent->SetState(EWallState::Completed);
        UpdateWidgetVisibility();
    }
    else
    {
        // Wall is too short, destroy it
        Destroy();
    }
}

void AWallActor::CancelDrawing()
{
    if (StateComponent->GetState() == EWallState::Drawing)
    {
        Destroy();
    }
}

void AWallActor::HandleEditModeChanged(EEditMode NewMode, EEditMode OldMode)
{
    if (NewMode == EEditMode::None)
    {
        if (StateComponent->GetState() == EWallState::Drawing)
        {
            CancelDrawing();
        }
        else if (StateComponent->IsSelected())
        {
            StateComponent->SetSelected(false);
        }
    }
}

void AWallActor::SubscribeToEditModeChanges(AIPDrawingModePawn* DrawingPawn)
{
    if(DrawingPawn)
    {
        OwningDrawingPawn = DrawingPawn;
        OwningDrawingPawn->OnEditModeChanged.AddDynamic(this, &AWallActor::HandleEditModeChanged);
    }
}

void AWallActor::UnsubscribeFromEditModeChanges(AIPDrawingModePawn* DrawingPawn)
{
    DrawingPawn->OnEditModeChanged.RemoveDynamic(this, &AWallActor::HandleEditModeChanged);
}

void AWallActor::OnWallStateChanged(EWallState NewState, EWallState OldState)
{
    UpdateWidgetVisibility();
    UpdateWidgets();
}

void AWallActor::OnSelectionChanged(bool bSelected)
{
    UpdateWidgetVisibility();
}

void AWallActor::OnWallClicked()
{
    if (StateComponent)
    {
        if (!StateComponent->IsSelected())
        {
            HandleSelection();
        }
        else 
        {
            HandleDeselection();
        }
    }
}

void AWallActor::OnWallHoverBegin()
{
    if (!StateComponent->IsSelected() && StateComponent->GetState() != EWallState::Drawing)
    {
        StateComponent->SetState(EWallState::Highlighted);
    }
}

void AWallActor::OnWallHoverEnd()
{
    if (StateComponent->GetState() == EWallState::Highlighted)
    {
        StateComponent->SetState(EWallState::Completed);
    }
}

void AWallActor::HandleSelection()
{
    if (!StateComponent)
        return;

    StateComponent->SetSelected(true);
    StateComponent->SetState(EWallState::Selected);
    OnWallSelected.Broadcast(this);

    // You might want to show UI widgets here
    if (StartCornerWidget)
    {
        StartCornerWidget->SetVisibility(true);
    }
    if (EndCornerWidget)
    {
        EndCornerWidget->SetVisibility(true);
    }
    if (MeasurementWidget)
    {
        MeasurementWidget->SetVisibility(true);
    }
}

void AWallActor::HandleDeselection()
{
    if (!StateComponent)
        return;

    StateComponent->SetSelected(false);
    StateComponent->SetState(EWallState::Completed);

    // Hide UI widgets
    if (StartCornerWidget)
    {
        StartCornerWidget->SetVisibility(false);
    }
    if (EndCornerWidget)
    {
        EndCornerWidget->SetVisibility(false);
    }
    if (MeasurementWidget)
    {
        MeasurementWidget->SetVisibility(false);
    }
}

bool AWallActor::IsWallSelected() const
{
    return StateComponent ? StateComponent->IsSelected() : false;
}

void AWallActor::OnWindowsModified()
{
    UpdateWidgets();
}

void AWallActor::OnGeometryChanged()
{
    UpdateWidgets();
}

void AWallActor::CreateWidgets()
{
    // Initialize corner resize widgets
    if (UCornerResizeWidget* StartWidget = Cast<UCornerResizeWidget>(StartCornerWidget->GetWidget()))
    {
        StartWidget->SetWallActor(this);
        StartWidget->SetCornerType(true);
    }

    if (UCornerResizeWidget* EndWidget = Cast<UCornerResizeWidget>(EndCornerWidget->GetWidget()))
    {
        EndWidget->SetWallActor(this);
        EndWidget->SetCornerType(false);
    }
}

void AWallActor::UpdateWidgets()
{
    if (!StateComponent->IsSelected() && 
        StateComponent->GetState() != EWallState::Drawing)
        return;

    UpdateWidgetTransforms();

    // Update measurement widget
    if (UWallMeasurementWidget* Widget = Cast<UWallMeasurementWidget>(MeasurementWidget->GetWidget()))
    {
        float Length = GeometryComponent->GetLength();
        Widget->SetMeasurement(Length);

        // Get screen positions for ruler
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            FVector2D StartScreen, EndScreen;
            PC->ProjectWorldLocationToScreen(GeometryComponent->GetStart(), StartScreen);
            PC->ProjectWorldLocationToScreen(GeometryComponent->GetEnd(), EndScreen);
            Widget->UpdateRulerTransform(StartScreen, EndScreen);
        }
    }
}

void AWallActor::UpdateWidgetVisibility()
{
    bool bShowWidgets = StateComponent->IsSelected() || 
                       StateComponent->GetState() == EWallState::Drawing;
    
    StartCornerWidget->SetVisibility(bShowWidgets);
    EndCornerWidget->SetVisibility(bShowWidgets);
    MeasurementWidget->SetVisibility(bShowWidgets);
}

void AWallActor::UpdateWidgetTransforms()
{
    // Update corner widget positions
    StartCornerWidget->SetWorldLocation(GeometryComponent->GetStart());
    EndCornerWidget->SetWorldLocation(GeometryComponent->GetEnd());

    // Update measurement widget position
    FVector Center = GeometryComponent->GetCenter();
    Center.Z += 50.0f; // Offset above the wall
    MeasurementWidget->SetWorldLocation(Center);
}