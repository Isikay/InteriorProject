#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteriorProject/Enums/InteriorTypes.h"
#include "WindowStateComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWindowStateChanged, EWindowEditState /*NewState*/, EWindowEditState /*OldState*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWindowSelectionChanged, bool /*bSelected*/);

UCLASS()
class INTERIORPROJECT_API UWindowStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWindowStateComponent();

	// State Interface
	void SetState(EWindowEditState NewState);
	EWindowEditState GetState() const { return CurrentState; }
	void SetSelected(bool bSelect);
	bool IsSelected() const { return bIsSelected; }

	// Helper functions
	bool IsInPlacementMode() const { return CurrentState == EWindowEditState::Placement; }
	bool IsMoving() const { return CurrentState == EWindowEditState::Moving; }
	bool IsResizing() const { return CurrentState == EWindowEditState::Resizing; }

	// Events
	FOnWindowStateChanged OnWindowStateChanged;
	FOnWindowSelectionChanged OnWindowSelectionChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(Transient)
	EWindowEditState CurrentState;

	UPROPERTY(Transient)
	bool bIsSelected;
};