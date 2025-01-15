#pragma once

#include "CoreMinimal.h"
#include "WallComponentBase.h"
#include "WallInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallHoverBegin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallHoverEnd);

UCLASS()
class INTERIORPROJECT_API UWallInteractionComponent : public UWallComponentBase
{
	GENERATED_BODY()

public:
	UWallInteractionComponent();

	// Input handlers
	UFUNCTION()
	void OnClicked(UPrimitiveComponent* ClickedComp, FKey ButtonPressed);
    
	UFUNCTION()
	void OnHoverBegin(UPrimitiveComponent* HoveredComp);
    
	UFUNCTION()
	void OnHoverEnd(UPrimitiveComponent* HoveredComp);

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Wall Events")
	FOnWallClicked OnWallClicked;

	UPROPERTY(BlueprintAssignable, Category = "Wall Events")
	FOnWallHoverBegin OnWallHoverBegin;

	UPROPERTY(BlueprintAssignable, Category = "Wall Events")
	FOnWallHoverEnd OnWallHoverEnd;

protected:
	virtual void OnInitialize() override;
	virtual void OnCleanup() override;

private:
	UPROPERTY()
	UPrimitiveComponent* InteractableComponent;

	void SetupInputBindings();
	void RemoveInputBindings();
	bool IsInteractionEnabled() const;
};