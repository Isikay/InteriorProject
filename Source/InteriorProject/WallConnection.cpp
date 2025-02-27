// Fill out your copyright notice in the Description page of Project Settings.


#include "WallConnection.h"
#include "Components/WidgetComponent.h"
#include "UI/ConnectedWallsWidget.h"


// Sets default values
AWallConnection::AWallConnection()
{
	ConnectionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WindowComponent"));
	ConnectionWidgetComponent->SetupAttachment(RootComponent);
}

void AWallConnection::SetWallManager(AFloorActor* Manager)
{
	OwningWallManager = Manager;
}

void AWallConnection::SetConnectedWalls(const TArray<FWallCorner>& Walls)
{
	ConnectedWalls = Walls;
}

void AWallConnection::HandlePressed()
{
	if(OwningWallManager)
	{
		
	}
}

void AWallConnection::HandleHovered()
{
	if(OwningWallManager)
	{
		
	}
}

void AWallConnection::HandleUnhovered()
{
	if(OwningWallManager)
	{
		
	}
}

// Called when the game starts or when spawned
void AWallConnection::BeginPlay()
{
	Super::BeginPlay();

	if(UConnectedWallsWidget* ConnectedWallsWidget = Cast<UConnectedWallsWidget>(ConnectionWidgetComponent->GetWidget()))
	{
		ConnectedWallsWidget->SetWallConnectionActor(this);
	}
}

