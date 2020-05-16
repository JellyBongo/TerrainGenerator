// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkProvider.h"
#include "Modifiers/RuntimeMeshModifierNormals.h"

UChunkProvider::UChunkProvider()
{
	MaterialScale = 0.01f;
}

UMaterialInterface* UChunkProvider::GetDisplayMaterial() const
{
	FScopeLock Lock(&PropertySyncRoot);
	return DisplayMaterial;
}

void UChunkProvider::SetChunkSize(int32 InChunkSize)
{
	ChunkSize = InChunkSize;
}

void UChunkProvider::SetCellSize(int32 InCellSize)
{
	CellSize = InCellSize;
}

void UChunkProvider::SetMaxHeight(int32 InMaxHeight)
{
	MaxHeight = InMaxHeight;
}

void UChunkProvider::SetStartCoords(FVector2D& InStartCoords)
{
	StartCoords = InStartCoords;
}

void UChunkProvider::SetDisplayMaterial(UMaterialInterface* InMaterial)
{
	FScopeLock Lock(&PropertySyncRoot);
	DisplayMaterial = InMaterial;
}

void UChunkProvider::CalculateBounds()
{
	int32 MaxCoordinateValue = ChunkSize * CellSize;
	FBox BoxBounds(FVector(StartCoords.X, StartCoords.Y, 0), FVector(StartCoords.X + MaxCoordinateValue, StartCoords.Y + MaxCoordinateValue, MaxHeight));
	LocalBounds = FBoxSphereBounds(BoxBounds);
}

FVector2D UChunkProvider::GetTextureCoordinates(int32 X, int32 Y) const
{
	return FVector2D(X / (MaterialScale * (ChunkSize - 1.0f)), Y / (MaterialScale * (ChunkSize - 1.0f)));
}

void UChunkProvider::AddCellToMeshData(int32 VertexIndex, FRuntimeMeshRenderableMeshData& MeshData) const
{
	MeshData.Triangles.Add(VertexIndex);
	MeshData.Triangles.Add(VertexIndex + 1);
	MeshData.Triangles.Add(VertexIndex + ChunkSize + 1);
	MeshData.Triangles.Add(VertexIndex);
	MeshData.Triangles.Add(VertexIndex + ChunkSize + 1);
	MeshData.Triangles.Add(VertexIndex + ChunkSize);
}

void UChunkProvider::Initialize_Implementation()
{
	SetupMaterialSlot(0, FName("Material"), DisplayMaterial);
	TArray<FRuntimeMeshLODProperties> LODs;
	FRuntimeMeshLODProperties LODProperties;
	LODProperties.ScreenSize = 0.0f;
	LODs.Add(LODProperties);
	ConfigureLODs(LODs);

	FRuntimeMeshSectionProperties Properties;
	Properties.bCastsShadow = true;
	Properties.bIsVisible = true;
	Properties.MaterialSlot = 0;
	Properties.bWants32BitIndices = true;
	Properties.UpdateFrequency = ERuntimeMeshUpdateFrequency::Infrequent;
	CreateSection(0, 0, Properties);
}

FBoxSphereBounds UChunkProvider::GetBounds_Implementation()
{
	return LocalBounds;
}

bool UChunkProvider::GetSectionMeshForLOD_Implementation(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData)
{
	check(SectionId == 0 && LODIndex == 0);

	// Add vertices and texture coordinates to MeshData
	for (int32 X = 0; X < ChunkSize; X++)
	{
		for (int32 Y = 0; Y < ChunkSize; Y++)
		{
			MeshData.Positions.Add(FVector(StartCoords.X + X * CellSize, StartCoords.Y + Y * CellSize, FMath::RandRange(0, MaxHeight)));
			MeshData.TexCoords.Add(GetTextureCoordinates(X, Y));
		}
	}

	// Add triangles to MeshData
	for (int32 VertexIndex = 0; VertexIndex < MeshData.Positions.Num(); VertexIndex++)
	{
		// If the vertex is not on the top border or the right border of the terrain
		if (VertexIndex % ChunkSize != ChunkSize - 1
			&& VertexIndex / ChunkSize != ChunkSize - 1)
		{
			AddCellToMeshData(VertexIndex, MeshData);
		}
	}

	URuntimeMeshModifierNormals::CalculateNormalsTangents(MeshData, false);

	return true;
}

bool UChunkProvider::IsThreadSafe_Implementation()
{
	return true;
}