#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "InteriorProject/Structs/Structs.h"
#include "ConnectedWallsWidget.generated.h"

class AWallConnection;
class UButton;

UCLASS()
class INTERIORPROJECT_API UConnectedWallsWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetWallConnectionActor(AWallConnection* WallConnectionPoint);

	void SetConnectedWalls(const TArray<FWallCorner>& Walls);

protected:
	
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* ConnectPointHandle;

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FLinearColor NormalColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FLinearColor HoverColor = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FLinearColor DragColor = FLinearColor(0.0f, 0.8f, 1.0f, 1.0f);

private:
    
	UPROPERTY()
	AWallConnection* OwningWallConnectionActor;
	
	UFUNCTION()
	void ResizeHandlePressed();
	
	UFUNCTION()
	void ResizeHandleHovered();
	
	UFUNCTION()
	void ResizeHandleUnhovered();

	TArray<FWallCorner> ConnectedWalls;
};