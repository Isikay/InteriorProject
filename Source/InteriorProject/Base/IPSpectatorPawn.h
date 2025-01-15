#pragma once

#include "CoreMinimal.h"
#include "IPBasePawn.h"
#include "InputActionValue.h"
#include "IPSpectatorPawn.generated.h"

UCLASS()
class INTERIORPROJECT_API AIPSpectatorPawn : public AIPBasePawn
{
	GENERATED_BODY()

public:
	AIPSpectatorPawn();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Input Actions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* MoveUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* CameraRotateAction;

	/** Movement Properties */
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MovementSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float VerticalMovementSpeed = 500.0f;

	/** Camera Properties */
	UPROPERTY(EditAnywhere, Category = "Camera")
	float RotationSpeed = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MinPitchAngle = -80.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MaxPitchAngle = 80.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float InitialFOV = 90.0f;

private:
	/** Input Functions */
	void Move(const FInputActionValue& Value);
	void MoveUp(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartCameraRotation();
	void StopCameraRotation();

	bool bIsRotating = false;
};