#pragma once

#include "CoreMinimal.h"
#include "InteriorTypes.generated.h"

// Main editing modes for the drawing pawn
UENUM(BlueprintType)
enum class EEditMode : uint8
{
    None             UMETA(DisplayName = "None"),
    WallDrawing      UMETA(DisplayName = "Wall Drawing"),
    Selection        UMETA(DisplayName = "Selection"),
    WindowPlacement  UMETA(DisplayName = "Window Placement")
};

// Camera/Pawn view modes
UENUM(BlueprintType)
enum class EPawnMode : uint8
{
    TopDown     UMETA(DisplayName = "Top Down"),
    FirstPerson UMETA(DisplayName = "First Person"),
    None        UMETA(DisplayName = "None")
};

// Wall states for visualization and interaction
UENUM(BlueprintType)
enum class EWallState : uint8
{
    None            UMETA(DisplayName = "None"),
    Drawing         UMETA(DisplayName = "Drawing"),
    Completed       UMETA(DisplayName = "Completed"),
    Selected        UMETA(DisplayName = "Selected"),
    WindowPlacement UMETA(DisplayName = "Window Placement"),
    Highlighted     UMETA(DisplayName = "Highlighted")
};

// Window edit states
UENUM(BlueprintType)
enum class EWindowEditState : uint8
{
    None        UMETA(DisplayName = "None"),
    Placement   UMETA(DisplayName = "Placement"),
    Moving      UMETA(DisplayName = "Moving"),
    Resizing    UMETA(DisplayName = "Resizing"),
    Selected    UMETA(DisplayName = "Selected"),
    Highlighted UMETA(DisplayName = "Highlighted")
};

// Wall side for window placement
UENUM(BlueprintType)
enum class EWallSide : uint8
{
    Front   UMETA(DisplayName = "Front"),
    Back    UMETA(DisplayName = "Back"),
    None    UMETA(DisplayName = "None")
};

// Resize handle types for walls
UENUM(BlueprintType)
enum class EResizeHandle : uint8
{
    None      UMETA(DisplayName = "None"),
    Length    UMETA(DisplayName = "Length"),
    Thickness UMETA(DisplayName = "Thickness")
};

// Collision channels for object types
UENUM(BlueprintType)
enum class EInteriorCollision : uint8
{
    Floor     UMETA(DisplayName = "Floor"),
    Wall      UMETA(DisplayName = "Wall"),
    Window    UMETA(DisplayName = "Window")
};