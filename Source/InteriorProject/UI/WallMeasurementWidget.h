#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WallMeasurementWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class INTERIORPROJECT_API UWallMeasurementWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMeasurement(float Length);
	void UpdateRulerTransform(const FVector2D& Start, const FVector2D& End);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* MeasurementText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* RulerLine;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* StartTick;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* EndTick;

	UPROPERTY(EditAnywhere, Category = "Appearance")
	FLinearColor RulerColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.8f);

	UPROPERTY(EditAnywhere, Category = "Appearance")
	float TextOffset = 20.0f;

private:
	void UpdateTextPosition(const FVector2D& Start, const FVector2D& End);
};