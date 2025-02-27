// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GUIMeasurement.generated.h"

class UImage;
class UEditableTextBox;
class USizeBox;

DECLARE_DYNAMIC_DELEGATE_OneParam( FOnMeasurementChanged, const float&, Size);
/**
 * 
 */
UCLASS()
class INTERIORPROJECT_API UGUIMeasurement : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativePreConstruct() override;

	virtual void NativeOnInitialized() override;

	void SetMeasurement(const float& Size);

	void SetFlipText(bool bFlip);

	FORCEINLINE UEditableTextBox* GetMeasurementText() const { return MeasurementText; }

	FORCEINLINE float GetMeasurement() const { return Measurement; }
	
	FOnMeasurementChanged OnMeasurementChanged;

protected:

	UFUNCTION()
	void OnTextChanged(const FText& Text);

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* MeasurementText;

	UPROPERTY(meta = (BindWidget))
	USizeBox* MeasurementSizeBox;

	/*UPROPERTY(meta = (BindWidget))
	UImage* MeasurementImage;*/

	UPROPERTY(meta = (BindWidget))
	UImage* LeftBarImage;

	UPROPERTY(meta = (BindWidget))
	UImage* RightBarImage;

	UPROPERTY(EditAnywhere)
	bool bFlipText = false;

	UPROPERTY(EditAnywhere)
	float Measurement = 0.0f;
};
