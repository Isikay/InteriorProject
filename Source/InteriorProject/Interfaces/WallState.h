#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteriorProject/Enums/InteriorTypes.h"
#include "WallState.generated.h"

UINTERFACE(MinimalAPI)
class UWallState : public UInterface
{
	GENERATED_BODY()
};

class INTERIORPROJECT_API IWallState
{
	GENERATED_BODY()

public:
	/** Set wall state */
	virtual void SetState(EWallState NewState) = 0;
    
	/** Get current wall state */
	virtual EWallState GetState() const = 0;
    
	/** Set wall selection state */
	virtual void SetSelected(bool bSelect) = 0;
    
	/** Check if wall is selected */
	virtual bool IsSelected() const = 0;
};