#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IPBasePawn.generated.h"

class UCameraComponent;

UCLASS()
class INTERIORPROJECT_API AIPBasePawn : public APawn
{
	GENERATED_BODY()

public:
	AIPBasePawn();

	// Get the pawn's input mapping context
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual UInputMappingContext* GetPawnMappingContext() const { return PawnMappingContext; }

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UCameraComponent* MainCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* PawnMappingContext;

	void AddInputMapping();
	void RemoveInputMapping();

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};