#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WallComponentInterface.generated.h"

UINTERFACE(MinimalAPI)
class UWallComponentInterface : public UInterface
{
	GENERATED_BODY()
};

class INTERIORPROJECT_API IWallComponentInterface
{
	GENERATED_BODY()

public:
	/** Initialize the component with owner wall */
	virtual void Initialize(class AWallActor* Owner) = 0;
    
	/** Cleanup component resources */
	virtual void Cleanup() = 0;
};