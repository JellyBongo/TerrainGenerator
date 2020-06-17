// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshProvider.h"
#include "FastNoise.h"
#include "BiomeMapper.h"
#include "ChunkProvider.generated.h"


/**
 * 
 */
UCLASS()
class TERRAINGENERATOR_API UChunkProvider : public URuntimeMeshProvider
{
	GENERATED_BODY()

private:
	mutable FCriticalSection PropertySyncRoot;

	UMaterialInterface* DisplayMaterial;

	/** Random seed for the generator */
	int32 Seed = 1337;

	/** Number of vertices in a side of one chunk
	* The chunk itself contains ChunkSize * ChunkSize vertices
	*/
	int32 ChunkSize;

	/** Distance between 2 adjacent vertices of the plain grid in world units */
	int32 CellSize;

	/** Maximal possible height of the terrain in world units*/
	int32 MaxHeight;

	float MaterialScale;
	
	/** Module that determines what biome is used in a point */
	UBiomeMapper* BiomeMapper;

	FBoxSphereBounds LocalBounds;
	FVector2D StartCoords;

	bool HasCollision = false;

public:

	UMaterialInterface* GetDisplayMaterial() const;
	
	void SetSeed(int32 InSeed);
	void SetChunkSize(int32 InChunkSize);
	void SetCellSize(int32 InCellSize);
	void SetMaxHeight(int32 InMaxHeight);
	void SetStartCoords(FVector2D& InStartCoords);
	void SetDisplayMaterial(UMaterialInterface* InMaterial);
	void SetHasCollision(bool InHasCollision);
	void SetBiomeMapper(UBiomeMapper* InBiomeMapper);

	void CalculateBounds();

protected:
	UChunkProvider();

	/** Calculates texture coordinates for the vertex */
	FVector2D GetTextureCoordinates(int32 X, int32 Y) const;

	/** Adds a rectangular cell (2 triangles) to MeshData */
	void AddCellToMeshData(int32 VertexIndex, FRuntimeMeshRenderableMeshData& MeshData) const;

	/** Adds a rectangular cell (2 triangles) to CollisionData */
	void AddCellToCollisionData(int32 VertexIndex, FRuntimeMeshCollisionData& CollisionData) const;

protected:
	// Below are some methodes of RuntimeMeshComponent we need to override
	void Initialize_Implementation() override;
	FBoxSphereBounds GetBounds_Implementation() override;
	bool GetSectionMeshForLOD_Implementation(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData) override;
	bool IsThreadSafe_Implementation() override;
	FRuntimeMeshCollisionSettings GetCollisionSettings_Implementation() override;
	bool HasCollisionMesh_Implementation() override;
	bool GetCollisionMesh_Implementation(FRuntimeMeshCollisionData& CollisionData) override;
};
