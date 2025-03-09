// GUISnapLine.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GUISnapLine.generated.h"

class UImage;

UCLASS()
class INTERIORPROJECT_API UGUISnapLine : public UUserWidget
{
	GENERATED_BODY()

public:
	// Snap çizgisini günceller
	void UpdateSnapLine(const FVector2D& StartPos, const FVector2D& EndPos);
	
	// Snap çizgisinin görünürlüğünü ayarlar
	void SetSnapLineVisible(bool bVisible);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UImage* SnapLineImage;

private:
	// Çizginin uzunluğunu ve açısını hesapla
	void CalculateLineTransform(const FVector2D& StartPos, const FVector2D& EndPos);
	
	// Çizgi kalınlığı
	UPROPERTY(EditAnywhere, Category = "Appearance")
	float LineThickness = 2.0f;
	
	// Çizgi rengi
	UPROPERTY(EditAnywhere, Category = "Appearance")
	FLinearColor LineColor = FLinearColor(0.0f, 1.0f, 0.0f, 0.7f); // Yarı saydam yeşil
	
	// Çizgi tipi (kesikli çizgi için)
	UPROPERTY(EditAnywhere, Category = "Appearance")
	bool bDashedLine = true;
	
	// Dash uzunluğu
	UPROPERTY(EditAnywhere, Category = "Appearance", meta = (EditCondition = "bDashedLine"))
	float DashLength = 5.0f;
	
	// Dash aralığı
	UPROPERTY(EditAnywhere, Category = "Appearance", meta = (EditCondition = "bDashedLine"))
	float DashGap = 5.0f;
};