#include "WallActor.h"
#include "Components/WallGeometryComponent.h"
#include "Components/WallStateComponent.h"
#include "Components/WallMeshComponent.h"
#include "Components/WallWindowComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/CornerResizeWidget.h"
#include "UI/WallMeasurementWidget.h"
#include "Base/IPDrawingModePawn.h"

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
    
    WindowComponent = CreateDefaultSubobject<UWallWindowComponent>(TEXT("WindowComponent"));

    // Create UI components
    StartCornerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("StartCornerWidget"));
    StartCornerWidget->SetupAttachment(RootComponent);
    StartCornerWidget->SetWidgetSpace(EWidgetSpace::Screen);
    StartCornerWidget->SetDrawSize(FVector2D(32.0f, 32.0f));

    EndCornerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("EndCornerWidget"));
    EndCornerWidget->SetupAttachment(RootComponent);
    EndCornerWidget->SetWidgetSpace(EWidgetSpace::Screen);
    EndCornerWidget->SetDrawSize(FVector2D(32.0f, 32.0f));

    MeasurementWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MeasurementWidget"));
    MeasurementWidget->SetupAttachment(RootComponent);
    MeasurementWidget->SetWidgetSpace(EWidgetSpace::Screen);
    MeasurementWidget->SetDrawSize(FVector2D(200.0f, 50.0f));
}

void AWallActor::BeginPlay()
{
    Super::BeginPlay();

    // Initialize components
    WindowComponent->Initialize(this);
    MeshComponent->Initialize(GeometryComponent, StateComponent, WindowComponent);

    // Setup event bindings
    StateComponent->OnWallStateChanged.AddUObject(this, &AWallActor::OnWallStateChanged);
    StateComponent->OnWallSelectionChanged.AddUObject(this, &AWallActor::OnSelectionChanged);
    
    // Setup mesh component interaction bindings
    if (MeshComponent)
    {
        MeshComponent->OnClicked.AddDynamic(this, &AWallActor::OnWallClicked);
        MeshComponent->OnBeginCursorOver.AddDynamic(this, &AWallActor::OnWallHoverBegin);
        MeshComponent->OnEndCursorOver.AddDynamic(this, &AWallActor::OnWallHoverEnd);
    }

    WindowComponent->OnWindowsModified.AddUObject(this, &AWallActor::OnWindowsModified);
    GeometryComponent->OnGeometryChanged.AddUObject(this, &AWallActor::OnGeometryChanged);

    // Create and setup widgets
    InitWidgets();
}

void AWallActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (OwningDrawingPawn)
    {
        UnsubscribeFromEditModeChanges(OwningDrawingPawn);
    }

    // Cleanup mesh component bindings
    if (MeshComponent)
    {
        MeshComponent->OnClicked.RemoveDynamic(this, &AWallActor::OnWallClicked);
        MeshComponent->OnBeginCursorOver.RemoveDynamic(this, &AWallActor::OnWallHoverBegin);
        MeshComponent->OnEndCursorOver.RemoveDynamic(this, &AWallActor::OnWallHoverEnd);
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

void AWallActor::EndDrawing()
{
    if (StateComponent->GetState() != EWallState::Drawing)
        return;
    

    if (GeometryComponent->GetLength() > GeometryComponent->MinWallLength)
    {
        StateComponent->SetState(EWallState::Completed);
        UpdateWidgetVisibility();
    }

    //Inform the player controller that the wall is completed
    if(OwningDrawingPawn)
    {
        OwningDrawingPawn->EndWallDrawing();
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
    if (DrawingPawn)
    {
        OwningDrawingPawn = DrawingPawn;
        OwningDrawingPawn->OnEditModeChanged.AddDynamic(this, &AWallActor::HandleEditModeChanged);
    }
}

void AWallActor::UnsubscribeFromEditModeChanges(AIPDrawingModePawn* DrawingPawn)
{
    DrawingPawn->OnEditModeChanged.RemoveDynamic(this, &AWallActor::HandleEditModeChanged);
}

void AWallActor::OnWallClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed)
{
    if (ButtonPressed == EKeys::LeftMouseButton)
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

void AWallActor::OnWallHoverBegin(UPrimitiveComponent* HoveredComp)
{
    if (!StateComponent->IsSelected() && StateComponent->GetState() != EWallState::Drawing)
    {
        StateComponent->SetState(EWallState::Highlighted);
    }
}

void AWallActor::OnWallHoverEnd(UPrimitiveComponent* HoveredComp)
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
}

void AWallActor::HandleDeselection()
{
    if (!StateComponent)
        return;

    StateComponent->SetSelected(false);
    StateComponent->SetState(EWallState::Completed);
}

bool AWallActor::IsWallSelected() const
{
    return StateComponent ? StateComponent->IsSelected() : false;
}

void AWallActor::OnWallStateChanged(EWallState NewState, EWallState OldState)
{
    UpdateWidgets();
}

void AWallActor::OnSelectionChanged(bool bSelected)
{
    UpdateWidgets();
}

void AWallActor::OnWindowsModified()
{
    UpdateWidgets();
}

void AWallActor::OnGeometryChanged()
{
    UpdateWidgets();
}

void AWallActor::InitWidgets()
{
    // Initialize corner resize widgets
    if (UCornerResizeWidget* StartWidget = Cast<UCornerResizeWidget>(StartCornerWidget->GetWidget()))
    {
        StartWidget->SetCornerType(true);
        StartWidget->SetWallActor(this);
    }

    if (UCornerResizeWidget* EndWidget = Cast<UCornerResizeWidget>(EndCornerWidget->GetWidget()))
    {
        EndWidget->SetCornerType(false);
        EndWidget->SetWallActor(this);
    }
}

void AWallActor::UpdateWidgets()
{
    UpdateWidgetVisibility();
    UpdateWidgetTransforms();
    UpdateMeaseurementWidget();
}

void AWallActor::UpdateMeaseurementWidget()
{
    // Update measurement widget
    if (UWallMeasurementWidget* Widget = Cast<UWallMeasurementWidget>(MeasurementWidget->GetWidget()))
    {
        float Length = GeometryComponent->GetLength();
        Widget->SetMeasurement(Length);

        // Get screen positions for ruler
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            FVector2D StartScreen, EndScreen;
            PC->ProjectWorldLocationToScreen(GeometryComponent->GetStart(), StartScreen);
            PC->ProjectWorldLocationToScreen(GeometryComponent->GetEnd(), EndScreen);
            Widget->UpdateRulerTransform(StartScreen, EndScreen);
        }
    }
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

void AWallActor::UpdateWidgetVisibility()
{
    bool bShowWidgets = StateComponent->IsSelected() || 
                       StateComponent->GetState() == EWallState::Drawing;
    
    StartCornerWidget->SetVisibility(bShowWidgets);
    EndCornerWidget->SetVisibility(bShowWidgets);
    MeasurementWidget->SetVisibility(bShowWidgets);
}
