// Fill out your copyright notice in the Description page of Project Settings.


#include "GUIMeasurement.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"


void UGUIMeasurement::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(MeasurementText && LeftBarImage && RightBarImage && MeasurementSizeBox)
	{
		SetFlipText(bFlipText);
		SetMeasurement(Measurement);
	}
}

void UGUIMeasurement::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MeasurementText->OnTextChanged.AddDynamic(this, &UGUIMeasurement::OnTextChanged);
}

void UGUIMeasurement::SetMeasurement(const float& Size)
{
	Measurement = Size;
	MeasurementSizeBox->SetWidthOverride(Size);
	LeftBarImage->SetDesiredSizeOverride(FVector2d(Size==0?0:2.0f, 20.0f));
	RightBarImage->SetDesiredSizeOverride(FVector2d(Size==0?0:2.0f, 20.0f));
	MeasurementText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Size*10)));
}

void UGUIMeasurement::SetFlipText(bool bFlip)
{
	bFlipText = bFlip;
	if(bFlipText)
	{
		MeasurementText->SetRenderTransformAngle(180.0f);
	}
	else
	{
		MeasurementText->SetRenderTransformAngle(0.0f);
	}
}

void UGUIMeasurement::OnTextChanged(const FText& Text)
{
	FString NewString = Text.ToString();
	FString FilteredString;
   
	for(TCHAR Character : NewString)
	{
		if(FChar::IsDigit(Character))
		{
			FilteredString.AppendChar(Character);
		}
	}
   
	if(FilteredString != NewString)
	{
		MeasurementText->SetText(FText::FromString(FilteredString));
	}
}
