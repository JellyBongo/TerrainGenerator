// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshProvider.h"
#include "ExampleTerrainProvider.generated.h"

/**
 * 
 */
UCLASS()
class TERRAINGENERATOR_API UExampleTerrainProvider : public URuntimeMeshProvider
{
	GENERATED_BODY()
	
private:
	mutable FCriticalSection PropertySyncRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintGetter = GetDisplayMaterial, BlueprintSetter = SetDisplayMaterial)
	UMaterialInterface* DisplayMaterial;

	/** Number of vertices in a side of one chunk
	* The chunk itself contains ChunkSize * ChunkSize vertices
	*/
	UPROPERTY(VisibleAnywhere)
	int32 ChunkSize;

	/** Distance between 2 adjacent vertices of the plain grid in world units */
	UPROPERTY(VisibleAnywhere)
	int32 CellSize;

	/** Maximal possible height of the terrain in world units*/
	UPROPERTY(VisibleAnywhere)
	int32 MaxHeight;

	UPROPERTY(VisibleAnywhere)
	float MaterialScale;

	FBoxSphereBounds LocalBounds;

	/*UPROPERTY(Category = "RuntimeMesh|Providers|Box", VisibleAnywhere, BlueprintGetter = GetBoxRadius, BlueprintSetter = SetBoxRadius)
	FVector BoxRadius;*/

public:
	UExampleTerrainProvider();

	/*UFUNCTION(Category = "RuntimeMesh|Providers|Box", BlueprintCallable)
	FVector GetBoxRadius() const;
	UFUNCTION(Category = "RuntimeMesh|Providers|Box", BlueprintCallable)
	void SetBoxRadius(const FVector& InRadius);*/

	UFUNCTION(BlueprintCallable)
	UMaterialInterface* GetDisplayMaterial() const;
	UFUNCTION(BlueprintCallable)
	void SetDisplayMaterial(UMaterialInterface* InMaterial);
	
protected:
	void CalculateBounds();

	/** Calculates texture coordinates for the vertex */
	UFUNCTION(BlueprintCallable)
	FVector2D GetTextureCoordinates(int32 X, int32 Y) const;

	/** Adds a rectangular cell (2 triangles) to MeshData */
	UFUNCTION(BlueprintCallable)
	void AddCellToMeshData(int32 VertexIndex, FRuntimeMeshRenderableMeshData& MeshData);

protected:
	void Initialize_Implementation() override;
	FBoxSphereBounds GetBounds_Implementation() override;
	bool GetSectionMeshForLOD_Implementation(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData& MeshData) override;
	bool IsThreadSafe_Implementation() override;
};
