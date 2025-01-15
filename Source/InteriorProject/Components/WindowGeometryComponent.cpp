#include "WindowGeometryComponent.h"

UWindowGeometryComponent::UWindowGeometryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
    
	WindowSize = FVector2D(100.0f, 150.0f);
	CurrentWallSide = EWallSide::None;
}

void UWindowGeometryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWindowGeometryComponent::SetSize(const FVector2D& NewSize)
{
	if (WindowSize != NewSize)
	{
		WindowSize = NewSize;
		ValidateSize();
		NotifyGeometryChanged();
	}
}

void UWindowGeometryComponent::SetWallSide(EWallSide Side)
{
	if (CurrentWallSide != Side)
	{
		CurrentWallSide = Side;
		NotifyGeometryChanged();
	}
}

void UWindowGeometryComponent::UpdatePosition(const FVector& NewPosition)
{
	if (GetOwner() && GetOwner()->GetActorLocation() != NewPosition)
	{
		GetOwner()->SetActorLocation(NewPosition);
		NotifyGeometryChanged();
	}
}

void UWindowGeometryComponent::ValidateSize()
{
	// Clamp size to minimum values
	WindowSize.X = FMath::Max(WindowSize.X, MinWidth);
	WindowSize.Y = FMath::Max(WindowSize.Y, MinHeight);
}

void UWindowGeometryComponent::NotifyGeometryChanged()
{
	OnGeometryChanged.Broadcast();
}