#pragma once

#include "CoreMinimal.h"
#include "Structs.generated.h"

class AWallActor;

USTRUCT()
struct FWallCorner
{
    GENERATED_BODY()
    
    UPROPERTY()
    AWallActor* Wall = nullptr;

    UPROPERTY()
    bool bIsStartConnected = false;
};