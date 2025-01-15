#pragma once

#include "CoreMinimal.h"
#include "WallComponentBase.h"
#include "InteriorProject/Interfaces/WallState.h"
#include "WallStateComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWallStateChanged, EWallState /*NewState*/, EWallState /*OldState*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWallSelectionChanged, bool /*bSelected*/);

UCLASS()
class INTERIORPROJECT_API UWallStateComponent : public UWallComponentBase, public IWallState
{
	GENERATED_BODY()

public:
	UWallStateComponent();

	// IWallState Interface
	virtual void SetState(EWallState NewState) override;
	virtual EWallState GetState() const override { return CurrentState; }
	virtual void SetSelected(bool bSelect) override;
	virtual bool IsSelected() const override { return bIsSelected; }

	// Event delegates
	FOnWallStateChanged OnWallStateChanged;
	FOnWallSelectionChanged OnWallSelectionChanged;

	// Helper functions
	bool IsInDrawingMode() const { return CurrentState == EWallState::Drawing; }
	bool IsCompleted() const { return CurrentState == EWallState::Completed; }
	bool IsHighlighted() const { return CurrentState == EWallState::Highlighted; }

protected:
	virtual void OnInitialize() override;
	virtual void OnCleanup() override;

private:
	UPROPERTY(Transient)
	EWallState CurrentState;

	UPROPERTY(Transient)
	bool bIsSelected;

	void NotifyStateChange(EWallState NewState, EWallState OldState);
	void NotifySelectionChange(bool bNewSelected);
};