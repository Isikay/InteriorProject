#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteriorProject/Interfaces/WallComponentInterface.h"
#include "WallComponentBase.generated.h"

UCLASS(Abstract)
class INTERIORPROJECT_API UWallComponentBase : public UActorComponent, public IWallComponentInterface
{
	GENERATED_BODY()

public:
	UWallComponentBase();

	// IWallComponent Interface
	virtual void Initialize(class AWallActor* Owner) override;
	virtual void Cleanup() override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	class AWallActor* WallOwner;

	// Optional override for child classes
	virtual void OnInitialize() {}
	virtual void OnCleanup() {}
};