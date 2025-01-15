#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProceduralMeshComponent.h"
#include "WallMeshGenerator.generated.h"

USTRUCT(BlueprintType)
struct FWallWindow
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Window")
    FVector2D Position;  // Relative position (X = distance from start, Y = height from bottom)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Window")
    FVector2D Size;      // Width and height

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Window")
    float Depth;         // Window frame depth

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Window")
    bool bIsBackFace;    // Whether window is on back face of wall

    FWallWindow()
        : Position(FVector2D::ZeroVector)
        , Size(FVector2D(100.0f, 150.0f))
        , Depth(20.0f)
        , bIsBackFace(false)
    {}
};

USTRUCT()
struct FWallMeshData
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FVector> Vertices;

    UPROPERTY()
    TArray<int32> Triangles;

    UPROPERTY()
    TArray<FVector2D> UVs;

    UPROPERTY()
    TArray<FVector> Normals;

    UPROPERTY()
    TArray<FProcMeshTangent> Tangents;
};

UCLASS()
class INTERIORPROJECT_API UWallMeshGenerator : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static FWallMeshData GenerateWallMesh(
        const FVector& Start,
        const FVector& End,
        float Height,
        float Thickness,
        const TArray<FWallWindow>& Windows);

private:
    static void CalculateDirectionVectors(
        const FVector& Start,
        const FVector& End,
        FVector& OutWallDirection,
        FVector& OutRightVector);

    static void AddRegularWallSection(
        FWallMeshData& MeshData,
        const FVector& Start,
        const FVector& End,
        float Height,
        float Thickness,
        const FVector& WallDirection,
        const FVector& RightVector);

    static void AddWindowSection(
        FWallMeshData& MeshData,
        const FVector& WallStart,
        const FWallWindow& Window,
        float Height,
        float Thickness,
        const FVector& WallDirection,
        const FVector& RightVector);

    static void AddWindowFrame(
        FWallMeshData& MeshData,
        const FWallWindow& Window,
        const FVector& WallStart,
        const FVector& WallDirection,
        const FVector& RightVector,
        float WallThickness);

    static void AddQuad(
        TArray<int32>& Triangles,
        int32 V0, int32 V1, int32 V2, int32 V3);

    static float GetUVScale() { return 1.0f / 100.0f; }  // 1 unit = 100cm
};