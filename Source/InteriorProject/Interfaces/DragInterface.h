#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DragInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDragInterface : public UInterface
{
	GENERATED_BODY()
};

class INTERIORPROJECT_API IDragInterface
{
	GENERATED_BODY()

public:

	// Inform new location
	virtual void Dragg(const FVector& DraggedDistance) = 0;
};