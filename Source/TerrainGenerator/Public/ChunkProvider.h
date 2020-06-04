// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshProvider.h"
#include "FastNoise.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintGetter = GetDisplayMaterial, BlueprintSetter = SetDisplayMaterial)
	UMaterialInterface* DisplayMaterial;

	/** FastNoise module */
	UFastNoise* Noise;

	/** Random seed for the generator */
	UPROPERTY(EditAnywhere)
	int32 Seed = 1337;

	/** Number of vertices in a side of one chunk
	* The chunk itself contains ChunkSize * ChunkSize vertices
	*/
	UPROPERTY(EditAnywhere)
	int32 ChunkSize;

	/** Distance between 2 adjacent vertices of the plain grid in world units */
	UPROPERTY(EditAnywhere)
	int32 CellSize;

	/** Maximal possible height of the terrain in world units*/
	UPROPERTY(EditAnywhere)
	int32 MaxHeight;

	UPROPERTY(VisibleAnywhere)
	float MaterialScale;
	
	FBoxSphereBounds LocalBounds;
	FVector2D StartCoords;

	bool HasCollision = false;

public:

	UFUNCTION(BlueprintCallable)
	UFastNoise* GetNoise() const;

	UFUNCTION(BlueprintCallable)
	UMaterialInterface* GetDisplayMaterial() const;

	UFUNCTION(BlueprintCallable)
	void SetSeed(int32 InSeed);

	UFUNCTION(BlueprintCallable)
	void SetChunkSize(int32 InChunkSize);

	UFUNCTION(BlueprintCallable)
	void SetCellSize(int32 InCellSize);

	UFUNCTION(BlueprintCallable)
	void SetMaxHeight(int32 InMaxHeight);

	UFUNCTION(BlueprintCallable)
	void SetStartCoords(FVector2D& InStartCoords);

	UFUNCTION(BlueprintCallable)
	void SetDisplayMaterial(UMaterialInterface* InMaterial);

	UFUNCTION(BlueprintCallable)
	void SetHasCollision(bool InHasCollision);

	void CalculateBounds();

protected:
	UChunkProvider();

	/** Calculates texture coordinates for the vertex */
	UFUNCTION(BlueprintCallable)
	FVector2D GetTextureCoordinates(int32 X, int32 Y) const;

	/** Adds a rectangular cell (2 triangles) to MeshData */
	UFUNCTION(BlueprintCallable)
	void AddCellToMeshData(int32 VertexIndex, FRuntimeMeshRenderableMeshData& MeshData) const;

	UFUNCTION(BlueprintCallable)
		void AddCellToCollisionData(int32 VertexIndex, FRuntimeMeshCollisionData& CollisionData) const;

protected:
	void Initialize_Implementation() override;
	FBoxSphereBounds GetBounds_Implementation() override;
	bool GetSectionMeshForLOD_Implementation(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData) override;
	bool IsThreadSafe_Implementation() override;
	FRuntimeMeshCollisionSettings GetCollisionSettings_Implementation() override;
	bool HasCollisionMesh_Implementation() override;
	bool GetCollisionMesh_Implementation(FRuntimeMeshCollisionData& CollisionData) override;
};
