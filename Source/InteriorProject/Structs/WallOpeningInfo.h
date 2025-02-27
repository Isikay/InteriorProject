#pragma once
#include "CoreMinimal.h"
#include "WallOpeningInfo.generated.h"

USTRUCT(BlueprintType)
struct INTERIORPROJECT_API FWallOpeningInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Size;

	FWallOpeningInfo() : Position(FVector2D(0,0)), Size(FVector2D(0,0)) {}
   
	FWallOpeningInfo(FVector2D InPosition, FVector2D InSize) 
		: Position(InPosition), Size(InSize) {}
	
	bool operator==(const FWallOpeningInfo& Other) const
	{
		return Position == Other.Position && Size == Other.Size;
	}
};

FORCEINLINE uint32 GetTypeHash(const FWallOpeningInfo& Info)
{
	uint32 Hash = HashCombine(GetTypeHash(Info.Position), GetTypeHash(Info.Size));
	return Hash;
}