// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshActor.h"
#include "BiomeMapper.h"
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
	/** Random seed for the generator */
	UPROPERTY(EditAnywhere)
	int32 Seed = 1337;

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

	TMap<FVector2D, URuntimeMeshComponent*> RenderedChunks;
	TMap<FVector2D, URuntimeMeshComponent*> ChunksWithBuiltCollisions;

	UBiomeMapper* BiomeMapper;
public:
	AProceduralTerrain();
	
	/** Called every frame */
	void Tick(float DeltaTime) override;

	/** Called when the game begins */
	void BeginPlay() override;

private:
	/** Get coordinates of chunks that are within RenderDistance from player */
	TArray<FVector2D> GetStartCoordsForNearbyChunks(const FVector& PlayerLocation) const;

	/** Get coordinates of chunks that need collisions to be set up */
	TArray<FVector2D> GetChunkCoordsForCollisionsSetup(const FVector& PlayerLocation) const;

	/** Returns true if chunk's location is further than RenderDistance from player */
	bool ChunkIsFarFromPlayer(FVector& PlayerLocation, FVector2D& ChunkLocation) const;

	/** Destroy chunks that are further than RenderDistance from player */
	void DestroyFarawayChunks(FVector& PlayerLocation);

	/** Spawn chunks within RenderDistance from player 
	*	Chunks that are already rendered will not be respawned	
	*/
	void SpawnNearbyChunks(FVector& PlayerLocation);

	/** Set up collisions for the current player location*/
	void SetupCollisions(FVector& PlayerLocation);
};
