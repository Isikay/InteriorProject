#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WallComponent.generated.h"

UINTERFACE(MinimalAPI)
class UWallComponent : public UInterface
{
	GENERATED_BODY()
};

class INTERIORPROJECT_API IWallComponent
{
	GENERATED_BODY()

public:
	virtual void Initialize(class AWallActor* Owner) = 0;
	virtual void Cleanup() = 0;
};