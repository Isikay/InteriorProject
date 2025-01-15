#include "WindowStateComponent.h"

UWindowStateComponent::UWindowStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentState = EWindowEditState::None;
	bIsSelected = false;
}

void UWindowStateComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWindowStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UWindowStateComponent::SetState(EWindowEditState NewState)
{
	if (CurrentState != NewState)
	{
		EWindowEditState OldState = CurrentState;
		CurrentState = NewState;

		// Update selection state based on new state
		if (NewState == EWindowEditState::None && bIsSelected)
		{
			SetSelected(false);
		}

		OnWindowStateChanged.Broadcast(NewState, OldState);
	}
}

void UWindowStateComponent::SetSelected(bool bSelect)
{
	if (bIsSelected != bSelect)
	{
		bIsSelected = bSelect;
		OnWindowSelectionChanged.Broadcast(bSelect);
	}
}