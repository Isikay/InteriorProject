#include "WallStateComponent.h"

UWallStateComponent::UWallStateComponent()
{
	CurrentState = EWallState::None;
	bIsSelected = false;
}

void UWallStateComponent::OnInitialize()
{
	// Initialize state-specific setup if needed
}

void UWallStateComponent::OnCleanup()
{
	// Reset state
	CurrentState = EWallState::None;
	bIsSelected = false;
}

void UWallStateComponent::SetState(EWallState NewState)
{
	if (CurrentState != NewState)
	{
		EWallState OldState = CurrentState;
		CurrentState = NewState;

		// Update selection state based on new wall state
		if (NewState != EWallState::Selected && bIsSelected)
		{
			SetSelected(false);
		}

		NotifyStateChange(NewState, OldState);
	}
}

void UWallStateComponent::SetSelected(bool bSelect)
{
	if (bIsSelected != bSelect)
	{
		bIsSelected = bSelect;
        
		// Update wall state based on selection
		if (bSelect)
		{
			SetState(EWallState::Selected);
		}
		else if (CurrentState == EWallState::Selected)
		{
			SetState(EWallState::Completed);
		}

		NotifySelectionChange(bSelect);
	}
}

void UWallStateComponent::NotifyStateChange(EWallState NewState, EWallState OldState)
{
	OnWallStateChanged.Broadcast(NewState, OldState);
}

void UWallStateComponent::NotifySelectionChange(bool bNewSelected)
{
	OnWallSelectionChanged.Broadcast(bNewSelected);
}