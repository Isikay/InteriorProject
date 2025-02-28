#pragma once

#include "CoreMinimal.h"
#include "Enums.generated.h"

// Main editing modes for the drawing pawn
UENUM(BlueprintType)
enum class EDrawingTools : uint8
{
    None             UMETA(DisplayName = "None"),
    WallDrawing      UMETA(DisplayName = "Wall Drawing"),
    WallUpdate       UMETA(DisplayName = "Wall Update"),
    RectangleDrawing UMETA(DisplayName = "Rectangle Drawing"),
    Placeable        UMETA(DisplayName = "Window Drawing"),
    Selection        UMETA(DisplayName = "Selection"),
    WallSpliting     UMETA(DisplayName = "Wall Clipping")
};
