// GUISnapLine.cpp
#include "GUISnapLine.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UGUISnapLine::NativeConstruct()
{
	Super::NativeConstruct();

	if (SnapLineImage)
	{
		// Material instance oluştur (Blueprint'te ayarlanan bir material olduğunu varsayıyoruz)
		UMaterialInstanceDynamic* DynamicMaterial = SnapLineImage->GetDynamicMaterial();
			
		// Material parametreleri ayarla
		DynamicMaterial->SetVectorParameterValue(TEXT("Color"), LineColor);
		DynamicMaterial->SetScalarParameterValue(TEXT("Thickness"), LineThickness);
			
		if (bDashedLine)
		{
			DynamicMaterial->SetScalarParameterValue(TEXT("DashEnabled"), 1.0f);
			DynamicMaterial->SetScalarParameterValue(TEXT("DashLength"), DashLength);
			DynamicMaterial->SetScalarParameterValue(TEXT("DashGap"), DashGap);
		}
		else
		{
			DynamicMaterial->SetScalarParameterValue(TEXT("DashEnabled"), 0.0f);
		}
		
		// Başlangıçta gizle
		SnapLineImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGUISnapLine::UpdateSnapLine(const FVector2D& StartPos, const FVector2D& EndPos)
{
	if (SnapLineImage)
	{
		// Çizginin transformunu hesapla
		CalculateLineTransform(StartPos, EndPos);
	}
}

void UGUISnapLine::SetSnapLineVisible(bool bVisible)
{
	if (SnapLineImage)
	{
		SnapLineImage->SetVisibility(bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}
}

void UGUISnapLine::CalculateLineTransform(const FVector2D& StartPos, const FVector2D& EndPos)
{
	if (SnapLineImage)
	{
		// Pozisyonu başlangıç noktasına ayarla
		SnapLineImage->SetRenderTransformPivot(FVector2D(0.0f, 0.5f)); // Sol ortadan pivot
		SnapLineImage->SetRenderTranslation(StartPos);
		
		// Uzunluğu hesapla
		float Length = FVector2D::Distance(StartPos, EndPos);
		SnapLineImage->SetDesiredSizeOverride(FVector2D(Length, LineThickness));
		
		// Açıyı hesapla
		FVector2D Direction = EndPos - StartPos;
		float Angle = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
		SnapLineImage->SetRenderTransformAngle(Angle);
	}
}