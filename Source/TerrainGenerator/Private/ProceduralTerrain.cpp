// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrain.h"
#include "ChunkProvider.h"
#include "Providers/RuntimeMeshProviderBox.h"
#include "Providers/RuntimeMeshProviderCollision.h"
#include "GameFramework/PlayerController.h"
#include "Engine/CollisionProfile.h"

AProceduralTerrain::AProceduralTerrain() : Material(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

int32 FloorByGrid(float Location, int32 Grid)
{
	int SnappedLocation = FMath::GridSnap(Location, Grid);
	// We need to snap location to the lower value of the grid so subtract 1 grid cell if it's snapped to the higher value
	return SnappedLocation > Location ? (SnappedLocation - Grid) : SnappedLocation;
}

TArray<FVector2D> AProceduralTerrain::GetStartCoordsForNearbyChunks(const FVector& PlayerLocation) const
{
	TArray<FVector2D> StartCoords;
	int32 ChunkSizeInWorldUnits = ChunkSize * CellSize;
	int32 XFloored = FloorByGrid(PlayerLocation.X, ChunkSizeInWorldUnits);
	int32 YFloored = FloorByGrid(PlayerLocation.Y, ChunkSizeInWorldUnits);

	for (int32 X = XFloored - ChunkSizeInWorldUnits * RenderDistance; X < XFloored + ChunkSizeInWorldUnits * (RenderDistance + 1); X += ChunkSizeInWorldUnits)
	{
		for (int32 Y = YFloored - ChunkSizeInWorldUnits * RenderDistance; Y < YFloored + ChunkSizeInWorldUnits * (RenderDistance + 1); Y += ChunkSizeInWorldUnits)
		{
			StartCoords.Add(FVector2D(X, Y));
		}
	}

	return StartCoords;
}

TArray<FVector2D> AProceduralTerrain::GetChunkCoordsForCollisionsSetup(const FVector& PlayerLocation) const
{
	int32 ChunkSizeInWorldUnits = ChunkSize * CellSize;
	int32 XFloored = FloorByGrid(PlayerLocation.X, ChunkSizeInWorldUnits);
	int32 YFloored = FloorByGrid(PlayerLocation.Y, ChunkSizeInWorldUnits);
	
	TArray<FVector2D> ChunkCoords = {
		FVector2D(XFloored, YFloored),
		FVector2D(XFloored + ChunkSizeInWorldUnits, YFloored),
		FVector2D(XFloored - ChunkSizeInWorldUnits, YFloored),
		FVector2D(XFloored, YFloored + ChunkSizeInWorldUnits),
		FVector2D(XFloored, YFloored - ChunkSizeInWorldUnits),
		FVector2D(XFloored + ChunkSizeInWorldUnits, YFloored + ChunkSizeInWorldUnits),
		FVector2D(XFloored + ChunkSizeInWorldUnits, YFloored - ChunkSizeInWorldUnits),
		FVector2D(XFloored - ChunkSizeInWorldUnits, YFloored - ChunkSizeInWorldUnits),
		FVector2D(XFloored - ChunkSizeInWorldUnits, YFloored + ChunkSizeInWorldUnits)
	};

	return ChunkCoords;
}

bool AProceduralTerrain::ChunkIsFarFromPlayer(FVector& PlayerLocation, FVector2D& ChunkLocation) const
{
	int32 ChunkCenterX = ChunkLocation.X + ((ChunkSize * CellSize) >> 1);
	int32 ChunkCenterY = ChunkLocation.Y + ((ChunkSize * CellSize) >> 1);
	float XDiff = PlayerLocation.X - ChunkCenterX;
	float YDiff = PlayerLocation.Y - ChunkCenterY;
	int32 MaxDistance = (RenderDistance + 1) * ChunkSize * CellSize;

	return FMath::Abs(XDiff) > MaxDistance || FMath::Abs(YDiff) > MaxDistance;
}

void AProceduralTerrain::DestroyFarawayChunks(FVector& PlayerLocation)
{
	TSet<FVector2D> KeysToRemove;
	for (TPair<FVector2D, URuntimeMeshComponent*> Chunk : RenderedChunks)
	{
		if (ChunkIsFarFromPlayer(PlayerLocation, Chunk.Key))
		{
			KeysToRemove.Add(Chunk.Key);
			Chunk.Value->DestroyComponent(true);
		}
	}

	for (FVector2D Key : KeysToRemove)
	{
		RenderedChunks.Remove(Key);
	}
}

void AProceduralTerrain::SpawnNearbyChunks(FVector& PlayerLocation)
{
	TArray<FVector2D> StartCoordsOfNearbyChunks = GetStartCoordsForNearbyChunks(PlayerLocation);
	
	// Create a provider and component for each nearby chunk that is not constructed yet
	for (FVector2D& StartCoords : StartCoordsOfNearbyChunks)
	{
		if (!RenderedChunks.Contains(StartCoords))
		{
			UChunkProvider* ChunkProvider = NewObject<UChunkProvider>(this);
			if (ChunkProvider)
			{
				ChunkProvider->SetBiomeMapper(BiomeMapper);

				ChunkProvider->SetSeed(Seed);
				ChunkProvider->SetDisplayMaterial(Material);
				ChunkProvider->SetChunkSize(ChunkSize);
				ChunkProvider->SetCellSize(CellSize);
				ChunkProvider->SetMaxHeight(MaxHeight);
				ChunkProvider->SetStartCoords(StartCoords);
				ChunkProvider->CalculateBounds();

				URuntimeMeshComponent* Chunk = NewObject<URuntimeMeshComponent>(this);
				Chunk->RegisterComponent();
				FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);
				Chunk->AttachToComponent(GetRuntimeMeshComponent(), Rules);
				Chunk->Initialize(ChunkProvider);
				RenderedChunks.Emplace(StartCoords, Chunk);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Chunk provider was not initialized"));
			}
		}
	}
}

void AProceduralTerrain::SetupCollisions(FVector& PlayerLocation)
{
	TArray<FVector2D> ChunkCoordsToSetupCollisions = GetChunkCoordsForCollisionsSetup(PlayerLocation);
	TArray<FVector2D> ChunksToDestroyCollisions;

	// For each chunk with built collisions
	// Destroy collision mesh if the chunk doesn't need it anymore
	// Exclude the chunk from ChunkCoordsToSetupCollisions if it already has collision mesh
	for (TPair<FVector2D, URuntimeMeshComponent*> Kv : ChunksWithBuiltCollisions)
	{
		FVector2D& ChunkCoords = Kv.Key;
		URuntimeMeshComponent* Chunk = Kv.Value;
		if (!ChunkCoordsToSetupCollisions.Contains(ChunkCoords))
		{
			((UChunkProvider*)(Chunk->GetProvider()))->SetHasCollision(false);
			Chunk->GetProvider()->MarkCollisionDirty();
			ChunksToDestroyCollisions.Add(ChunkCoords);
		}
		else
		{
			ChunkCoordsToSetupCollisions.Remove(ChunkCoords);
		}
	}
	
	// Clean the hashmap
	for (FVector2D Coords : ChunksToDestroyCollisions)
	{
		ChunksWithBuiltCollisions.Remove(Coords);
	}

	// Setup necessary collision meshes
	for (FVector2D ChunkCoord : ChunkCoordsToSetupCollisions)
	{
		URuntimeMeshComponent* Chunk = RenderedChunks[ChunkCoord];
		((UChunkProvider*)(Chunk->GetProvider()))->SetHasCollision(true);
		Chunk->GetProvider()->MarkCollisionDirty();
		ChunksWithBuiltCollisions.Emplace(ChunkCoord, Chunk);
	}	
}

void AProceduralTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	DestroyFarawayChunks(PlayerLocation);

	SpawnNearbyChunks(PlayerLocation);

	SetupCollisions(PlayerLocation);
}

void AProceduralTerrain::BeginPlay()
{
	Super::BeginPlay();
	BiomeMapper = NewObject<UBiomeMapper>(this, "BiomeMapper");
	BiomeMapper->SetSeed(Seed);
	BiomeMapper->SetMaxHeight(MaxHeight);
}
