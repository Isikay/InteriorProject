#include "IPBasePawn.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"

AIPBasePawn::AIPBasePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create root component if not created
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	}
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(RootComponent);
}

void AIPBasePawn::BeginPlay()
{
	Super::BeginPlay();
}

void AIPBasePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AddInputMapping();
}

void AIPBasePawn::UnPossessed()
{
	RemoveInputMapping();
	Super::UnPossessed();
}

void AIPBasePawn::AddInputMapping()
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PawnMappingContext, 1);
		}
	}
}

void AIPBasePawn::RemoveInputMapping()
{
	// Remove Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(PawnMappingContext);
		}
	}
}

void AIPBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIPBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}