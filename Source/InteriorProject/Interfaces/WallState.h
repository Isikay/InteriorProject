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
	virtual void SetState(EWallState NewState) = 0;
	virtual EWallState GetState() const = 0;
	virtual void SetSelected(bool bSelect) = 0;
	virtual bool IsSelected() const = 0;
};