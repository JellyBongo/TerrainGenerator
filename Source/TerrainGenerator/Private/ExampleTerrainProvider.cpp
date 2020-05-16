// Fill out your copyright notice in the Description page of Project Settings.


#include "ExampleTerrainProvider.h"
#include "Modifiers/RuntimeMeshModifierNormals.h"

UExampleTerrainProvider::UExampleTerrainProvider()
{
	ChunkSize = 32;
	CellSize = 1000;
	MaxHeight = 2000;
	MaterialScale = 0.01f;
	CalculateBounds();
}

//void UExampleTerrainProvider::SetBoxRadius(const FVector& InRadius)
//{
//	FScopeLock Lock(&PropertySyncRoot);
//	BoxRadius = InRadius;
//
//	MarkAllLODsDirty();
//}

UMaterialInterface* UExampleTerrainProvider::GetDisplayMaterial() const
{
	FScopeLock Lock(&PropertySyncRoot);
	return DisplayMaterial;
}

void UExampleTerrainProvider::SetDisplayMaterial(UMaterialInterface* InMaterial)
{
	FScopeLock Lock(&PropertySyncRoot);
	DisplayMaterial = InMaterial;
}

void UExampleTerrainProvider::CalculateBounds()
{
	int32 MaxCoordinateValue = ChunkSize * CellSize;
	LocalBounds = FBoxSphereBounds(FBox(FVector(0, 0, 0), FVector(MaxCoordinateValue, MaxCoordinateValue, MaxHeight)));
}

FVector2D UExampleTerrainProvider::GetTextureCoordinates(int32 X, int32 Y) const
{
	return FVector2D(X / (MaterialScale * (ChunkSize - 1.0f)), Y / (MaterialScale * (ChunkSize - 1.0f)));
}

void UExampleTerrainProvider::AddCellToMeshData(int32 VertexIndex, FRuntimeMeshRenderableMeshData& MeshData)
{
	MeshData.Triangles.Add(VertexIndex);
	MeshData.Triangles.Add(VertexIndex + 1);
	MeshData.Triangles.Add(VertexIndex + ChunkSize + 1);
	MeshData.Triangles.Add(VertexIndex);
	MeshData.Triangles.Add(VertexIndex + ChunkSize + 1);
	MeshData.Triangles.Add(VertexIndex + ChunkSize);
}

void UExampleTerrainProvider::Initialize_Implementation()
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
	Properties.UpdateFrequency = ERuntimeMeshUpdateFrequency::Average;
	CreateSection(0, 0, Properties);
}

FBoxSphereBounds UExampleTerrainProvider::GetBounds_Implementation()
{
	return LocalBounds;
}

bool UExampleTerrainProvider::GetSectionMeshForLOD_Implementation(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData)
{
	check(SectionId == 0 && LODIndex == 0);

	// Add vertices and texture coordinates to MeshData
	for (int32 X = 0; X < ChunkSize; X++)
	{
		for (int32 Y = 0; Y < ChunkSize; Y++)
		{
			MeshData.Positions.Add(FVector(X * CellSize, Y * CellSize, FMath::RandRange(0, MaxHeight)));
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

bool UExampleTerrainProvider::IsThreadSafe_Implementation()
{
	return true;
}
