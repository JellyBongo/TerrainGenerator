// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkProvider.h"
#include "Modifiers/RuntimeMeshModifierNormals.h"

UChunkProvider::UChunkProvider()
{
	MaterialScale = 0.01f;
	Noise = NewObject<UFastNoise>(this, "NoiseGenerator");
}

UMaterialInterface* UChunkProvider::GetDisplayMaterial() const
{
	FScopeLock Lock(&PropertySyncRoot);
	return DisplayMaterial;
}

UFastNoise* UChunkProvider::GetNoise() const
{
	FScopeLock Lock(&PropertySyncRoot);
	return Noise;
}

void UChunkProvider::SetSeed(int32 InSeed)
{
	Seed = InSeed;
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

void UChunkProvider::SetHasCollision(bool InHasCollision)
{
	HasCollision = InHasCollision;
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
	MeshData.Triangles.Add(VertexIndex + ChunkSize + 2);
	MeshData.Triangles.Add(VertexIndex);
	MeshData.Triangles.Add(VertexIndex + ChunkSize + 2);
	MeshData.Triangles.Add(VertexIndex + ChunkSize + 1);
}

void UChunkProvider::AddCellToCollisionData(int32 VertexIndex, FRuntimeMeshCollisionData& CollisionData) const
{
	CollisionData.Triangles.Add(VertexIndex, VertexIndex + 1, VertexIndex + ChunkSize + 2);
	CollisionData.Triangles.Add(VertexIndex, VertexIndex + ChunkSize + 2, VertexIndex + ChunkSize + 1);
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

	MarkCollisionDirty();
}

FBoxSphereBounds UChunkProvider::GetBounds_Implementation()
{
	return LocalBounds;
}

bool UChunkProvider::GetSectionMeshForLOD_Implementation(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData)
{
	check(SectionId == 0 && LODIndex == 0);

	// Generating a square grid of (ChunkSize + 1) * (ChunkSize + 1) vertices
	// The vertices will have height determined by some noise function
	// Each 4 adjasent vertices will be connected into a cell (2 polygons)

	// Add vertices and texture coordinates to MeshData
	for (int32 X = 0; X <= ChunkSize; X++)
	{
		for (int32 Y = 0; Y <= ChunkSize; Y++)
		{
			int32 VertexX = StartCoords.X + X * CellSize;
			int32 VertexY = StartCoords.Y + Y * CellSize;
			int32 VertexZ = (Noise->GetNoise(VertexX, VertexY) + 1) * MaxHeight / 2;
			MeshData.Positions.Add(FVector(VertexX, VertexY, VertexZ));
			MeshData.TexCoords.Add(GetTextureCoordinates(X, Y));
		}
	}

	// Add triangles to MeshData
	for (int32 VertexIndex = 0; VertexIndex < MeshData.Positions.Num(); VertexIndex++)
	{
		// If the vertex is not on the top border or the right border of the terrain
		if (VertexIndex % (ChunkSize + 1) != ChunkSize
			&& VertexIndex / (ChunkSize + 1) != ChunkSize)
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

FRuntimeMeshCollisionSettings UChunkProvider::GetCollisionSettings_Implementation()
{
	FRuntimeMeshCollisionSettings Settings;
	Settings.bUseAsyncCooking = true;
	Settings.bUseComplexAsSimple = true;

	return Settings;
}

bool UChunkProvider::HasCollisionMesh_Implementation()
{
	return HasCollision;
}

bool UChunkProvider::GetCollisionMesh_Implementation(FRuntimeMeshCollisionData& CollisionData)
{
	for (int32 X = 0; X <= ChunkSize; X++)
	{
		for (int32 Y = 0; Y <= ChunkSize; Y++)
		{
			int32 VertexX = StartCoords.X + X * CellSize;
			int32 VertexY = StartCoords.Y + Y * CellSize;
			int32 VertexZ = (Noise->GetNoise(VertexX, VertexY) + 1) * MaxHeight / 2;
			CollisionData.Vertices.Add(FVector(VertexX, VertexY, VertexZ));
		}
	}

	// Add triangles to MeshData
	for (int32 VertexIndex = 0; VertexIndex < CollisionData.Vertices.Num(); VertexIndex++)
	{
		// If the vertex is not on the top border or the right border of the terrain
		if (VertexIndex % (ChunkSize + 1) != ChunkSize
			&& VertexIndex / (ChunkSize + 1) != ChunkSize)
		{
			AddCellToCollisionData(VertexIndex, CollisionData);
		}
	}

	return true;
}
