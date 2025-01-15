#include "WallMeshGenerator.h"

void UWallMeshGenerator::CalculateDirectionVectors(
    const FVector& Start,
    const FVector& End,
    FVector& OutWallDirection,
    FVector& OutRightVector)
{
    // Calculate wall direction
    FVector Delta = End - Start;
    Delta.Z = 0;  // Ensure wall direction is horizontal
    
    if (Delta.IsNearlyZero())
    {
        OutWallDirection = FVector::ForwardVector;
        OutRightVector = FVector::RightVector;
        return;
    }
    
    OutWallDirection = Delta.GetSafeNormal();
    OutRightVector = FVector::CrossProduct(OutWallDirection, FVector::UpVector).GetSafeNormal();
}

FWallMeshData UWallMeshGenerator::GenerateWallMesh(
    const FVector& Start,
    const FVector& End,
    float Height,
    float Thickness,
    const TArray<FWallWindow>& Windows)
{
    FWallMeshData MeshData;
    
    FVector WallDirection, RightVector;
    CalculateDirectionVectors(Start, End, WallDirection, RightVector);

    float WallLength = (End - Start).Size();
    if (WallLength < KINDA_SMALL_NUMBER)
        return MeshData;

    // If no windows, generate a simple wall
    if (Windows.Num() == 0)
    {
        AddRegularWallSection(MeshData, Start, End, Height, Thickness, WallDirection, RightVector);
        return MeshData;
    }

    // Sort windows by X position
    TArray<FWallWindow> SortedWindows = Windows;
    SortedWindows.Sort([](const FWallWindow& A, const FWallWindow& B) {
        return A.Position.X < B.Position.X;
    });

    // Generate wall sections between windows
    float LastX = 0.0f;
    for (const FWallWindow& Window : SortedWindows)
    {
        // Add wall segment before window if needed
        if (Window.Position.X > LastX)
        {
            FVector SegStart = Start + WallDirection * LastX;
            FVector SegEnd = Start + WallDirection * Window.Position.X;
            AddRegularWallSection(MeshData, SegStart, SegEnd, Height, Thickness, WallDirection, RightVector);
        }

        // Add window section and frame
        AddWindowSection(MeshData, Start, Window, Height, Thickness, WallDirection, RightVector);
        AddWindowFrame(MeshData, Window, Start, WallDirection, RightVector, Thickness);

        LastX = Window.Position.X + Window.Size.X;
    }

    // Add final wall segment if needed
    if (LastX < WallLength)
    {
        FVector SegStart = Start + WallDirection * LastX;
        FVector SegEnd = End;
        AddRegularWallSection(MeshData, SegStart, SegEnd, Height, Thickness, WallDirection, RightVector);
    }

    return MeshData;
}

void UWallMeshGenerator::AddRegularWallSection(
    FWallMeshData& MeshData,
    const FVector& Start,
    const FVector& End,
    float Height,
    float Thickness,
    const FVector& WallDirection,
    const FVector& RightVector)
{
    float HalfThickness = Thickness * 0.5f;

    // Calculate vertices
    FVector FL_Bottom = Start - RightVector * HalfThickness;
    FVector FR_Bottom = End - RightVector * HalfThickness;
    FVector FL_Top = FL_Bottom + FVector(0, 0, Height);
    FVector FR_Top = FR_Bottom + FVector(0, 0, Height);

    FVector BL_Bottom = Start + RightVector * HalfThickness;
    FVector BR_Bottom = End + RightVector * HalfThickness;
    FVector BL_Top = BL_Bottom + FVector(0, 0, Height);
    FVector BR_Top = BR_Bottom + FVector(0, 0, Height);

    // Store base index
    int32 BaseIndex = MeshData.Vertices.Num();

    // Add vertices
    MeshData.Vertices.Add(FL_Bottom); // 0
    MeshData.Vertices.Add(FR_Bottom); // 1
    MeshData.Vertices.Add(FL_Top);    // 2
    MeshData.Vertices.Add(FR_Top);    // 3
    MeshData.Vertices.Add(BL_Bottom); // 4
    MeshData.Vertices.Add(BR_Bottom); // 5
    MeshData.Vertices.Add(BL_Top);    // 6
    MeshData.Vertices.Add(BR_Top);    // 7

    // Add triangles for all faces
    // Front face
    AddQuad(MeshData.Triangles, BaseIndex + 0, BaseIndex + 1, BaseIndex + 2, BaseIndex + 3);
    // Back face
    AddQuad(MeshData.Triangles, BaseIndex + 5, BaseIndex + 4, BaseIndex + 7, BaseIndex + 6);
    // Left side
    AddQuad(MeshData.Triangles, BaseIndex + 4, BaseIndex + 0, BaseIndex + 6, BaseIndex + 2);
    // Right side
    AddQuad(MeshData.Triangles, BaseIndex + 1, BaseIndex + 5, BaseIndex + 3, BaseIndex + 7);
    // Top
    AddQuad(MeshData.Triangles, BaseIndex + 2, BaseIndex + 3, BaseIndex + 6, BaseIndex + 7);
    // Bottom
    AddQuad(MeshData.Triangles, BaseIndex + 0, BaseIndex + 1, BaseIndex + 4, BaseIndex + 5);

    // Calculate UVs
    float Length = (End - Start).Size();
    float UVScale = GetUVScale();

    MeshData.UVs.Add(FVector2D(0, 0));
    MeshData.UVs.Add(FVector2D(Length * UVScale, 0));
    MeshData.UVs.Add(FVector2D(0, Height * UVScale));
    MeshData.UVs.Add(FVector2D(Length * UVScale, Height * UVScale));

    MeshData.UVs.Add(FVector2D(0, 0));
    MeshData.UVs.Add(FVector2D(Length * UVScale, 0));
    MeshData.UVs.Add(FVector2D(0, Height * UVScale));
    MeshData.UVs.Add(FVector2D(Length * UVScale, Height * UVScale));

    // Add normals
    for (int32 i = 0; i < 4; ++i)
        MeshData.Normals.Add(-RightVector);
    for (int32 i = 0; i < 4; ++i)
        MeshData.Normals.Add(RightVector);

    // Add tangents
    for (int32 i = 0; i < 8; ++i)
    {
        MeshData.Tangents.Add(FProcMeshTangent(WallDirection, false));
    }
}

void UWallMeshGenerator::AddWindowSection(
    FWallMeshData& MeshData,
    const FVector& WallStart,
    const FWallWindow& Window,
    float Height,
    float Thickness,
    const FVector& WallDirection,
    const FVector& RightVector)
{
    // Add wall section below window if needed
    if (Window.Position.Y > 0)
    {
        FVector BottomStart = WallStart + WallDirection * Window.Position.X;
        FVector BottomEnd = BottomStart + WallDirection * Window.Size.X;
        AddRegularWallSection(MeshData, BottomStart, BottomEnd, Window.Position.Y, Thickness, WallDirection, RightVector);
    }

    // Add wall section above window if needed
    float WindowTop = Window.Position.Y + Window.Size.Y;
    if (WindowTop < Height)
    {
        FVector TopStart = WallStart + WallDirection * Window.Position.X + FVector(0, 0, WindowTop);
        FVector TopEnd = TopStart + WallDirection * Window.Size.X;
        float RemainingHeight = Height - WindowTop;
        AddRegularWallSection(MeshData, TopStart, TopEnd, RemainingHeight, Thickness, WallDirection, RightVector);
    }
}

void UWallMeshGenerator::AddWindowFrame(
    FWallMeshData& MeshData,
    const FWallWindow& Window,
    const FVector& WallStart,
    const FVector& WallDirection,
    const FVector& RightVector,
    float WallThickness)
{
    const float FrameThickness = 5.0f;
    const float FrameDepth = Window.Depth;
    
    FVector WindowStart = WallStart + WallDirection * Window.Position.X;
    FVector WindowEnd = WindowStart + WallDirection * Window.Size.X;
    
    float WindowBottom = Window.Position.Y;
    float WindowTop = WindowBottom + Window.Size.Y;

    // Add top frame
    AddRegularWallSection(MeshData,
        WindowStart + FVector(0, 0, WindowTop),
        WindowEnd + FVector(0, 0, WindowTop),
        FrameThickness,
        WallThickness,
        WallDirection,
        RightVector);

    // Add bottom frame
    AddRegularWallSection(MeshData,
        WindowStart + FVector(0, 0, WindowBottom - FrameThickness),
        WindowEnd + FVector(0, 0, WindowBottom - FrameThickness),
        FrameThickness,
        WallThickness,
        WallDirection,
        RightVector);

    // Add side frames
    AddRegularWallSection(MeshData,
        WindowStart + FVector(0, 0, WindowBottom),
        WindowStart + FVector(0, 0, WindowTop),
        WindowTop - WindowBottom,
        FrameThickness,
        WallDirection,
        RightVector);

    AddRegularWallSection(MeshData,
        WindowEnd + FVector(0, 0, WindowBottom),
        WindowEnd + FVector(0, 0, WindowTop),
        WindowTop - WindowBottom,
        FrameThickness,
        WallDirection,
        RightVector);
}

void UWallMeshGenerator::AddQuad(
    TArray<int32>& Triangles,
    int32 V0,
    int32 V1,
    int32 V2,
    int32 V3)
{
    Triangles.Add(V0);
    Triangles.Add(V1);
    Triangles.Add(V2);
    Triangles.Add(V1);
    Triangles.Add(V3);
    Triangles.Add(V2);
}