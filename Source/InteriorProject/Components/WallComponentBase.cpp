#include "WallComponentBase.h"

UWallComponentBase::UWallComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;
	WallOwner = nullptr;
}

void UWallComponentBase::BeginPlay()
{
	Super::BeginPlay();
}

void UWallComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Cleanup();
	Super::EndPlay(EndPlayReason);
}

void UWallComponentBase::Initialize(AWallActor* Owner)
{
	WallOwner = Owner;
	if (WallOwner)
	{
		OnInitialize();
	}
}

void UWallComponentBase::Cleanup()
{
	OnCleanup();
	WallOwner = nullptr;
}