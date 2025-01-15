#include "WallInteractionComponent.h"
#include "WallStateComponent.h"
#include "InteriorProject/WallActor.h"

UWallInteractionComponent::UWallInteractionComponent()
{
    InteractableComponent = nullptr;
}

void UWallInteractionComponent::OnInitialize()
{
    InteractableComponent = WallOwner->FindComponentByClass<UPrimitiveComponent>();
    SetupInputBindings();
}

void UWallInteractionComponent::OnCleanup()
{
    RemoveInputBindings();
    InteractableComponent = nullptr;
}

void UWallInteractionComponent::SetupInputBindings()
{
    if (InteractableComponent)
    {
        InteractableComponent->OnClicked.AddDynamic(this, &UWallInteractionComponent::OnClicked);
        InteractableComponent->OnBeginCursorOver.AddDynamic(this, &UWallInteractionComponent::OnHoverBegin);
        InteractableComponent->OnEndCursorOver.AddDynamic(this, &UWallInteractionComponent::OnHoverEnd);
    }
}

void UWallInteractionComponent::RemoveInputBindings()
{
    if (InteractableComponent)
    {
        InteractableComponent->OnClicked.RemoveDynamic(this, &UWallInteractionComponent::OnClicked);
        InteractableComponent->OnBeginCursorOver.RemoveDynamic(this, &UWallInteractionComponent::OnHoverBegin);
        InteractableComponent->OnEndCursorOver.RemoveDynamic(this, &UWallInteractionComponent::OnHoverEnd);
    }
}

void UWallInteractionComponent::OnClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed)
{
    if (!IsInteractionEnabled())
        return;

    if (ButtonPressed == EKeys::LeftMouseButton)
    {
        OnWallClicked.Broadcast();
    }
}

void UWallInteractionComponent::OnHoverBegin(UPrimitiveComponent* HoveredComp)
{
    if (!IsInteractionEnabled())
        return;

    OnWallHoverBegin.Broadcast();
}

void UWallInteractionComponent::OnHoverEnd(UPrimitiveComponent* HoveredComp)
{
    if (!IsInteractionEnabled())
        return;

    OnWallHoverEnd.Broadcast();
}

bool UWallInteractionComponent::IsInteractionEnabled() const
{
    if (!WallOwner)
        return false;

    if (UWallStateComponent* StateComponent = WallOwner->FindComponentByClass<UWallStateComponent>())
    {
        EWallState CurrentState = StateComponent->GetState();

        // Disable interaction during drawing or when in special states
        if (CurrentState == EWallState::Drawing)
            return false;

        // Check if the wall is in a state that allows interaction
        return (CurrentState == EWallState::Completed ||
                CurrentState == EWallState::Selected ||
                CurrentState == EWallState::Highlighted);
    }

    return false;
}