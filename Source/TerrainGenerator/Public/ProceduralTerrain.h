// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshActor.h"
#include "ProceduralTerrain.generated.h"

/**
 * 
 */
UCLASS()
class TERRAINGENERATOR_API AProceduralTerrain : public ARuntimeMeshActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

private:
	/** Number of vertices in a side of one chunk
	* The chunk itself contains ChunkSize * ChunkSize vertices
	*/
	UPROPERTY(EditAnywhere)
	int32 ChunkSize = 32;

	/** Distance between 2 adjacent vertices of the plain grid in world units */
	UPROPERTY(EditAnywhere)
	int32 CellSize = 1000;

	/** Maximal possible height of the terrain in world units*/
	UPROPERTY(EditAnywhere)
	int32 MaxHeight = 1500;

	/** Number of rendering chunks */
	UPROPERTY(EditAnywhere)
	int32 RenderDistance = 2;

	TSet<FVector2D> RenderedChunks;

public:
	AProceduralTerrain();

	void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//void OnConstruction(const FTransform& Transform) override;

private:
	TArray<FVector2D> GetStartCoordsForAdjacentChunks(const FVector& PlayerLocation) const;
};
