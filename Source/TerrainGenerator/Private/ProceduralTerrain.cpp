// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrain.h"
#include "ChunkProvider.h"
#include "Providers/RuntimeMeshProviderBox.h"
#include "Providers/RuntimeMeshProviderCollision.h"
//#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/CollisionProfile.h"

AProceduralTerrain::AProceduralTerrain() : Material(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AProceduralTerrain::BeginPlay()
{
	Super::BeginPlay();

}

int32 FloorByGrid(float Location, int32 Grid)
{
	int SnappedLocation = FMath::GridSnap(Location, Grid);
	return SnappedLocation > Location ? (SnappedLocation - Grid) : SnappedLocation;
}

TArray<FVector2D> AProceduralTerrain::GetStartCoordsForAdjacentChunks(const FVector& PlayerLocation) const
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

bool AProceduralTerrain::ChunkIsFarFromPlayer(FVector& PlayerLocation, FVector2D& ChunkLocation) const
{
	int32 ChunkCenterX = ChunkLocation.X + ((ChunkSize * CellSize) >> 1);
	int32 ChunkCenterY = ChunkLocation.Y + ((ChunkSize * CellSize) >> 1);
	float XDiff = PlayerLocation.X - ChunkCenterX;
	float YDiff = PlayerLocation.Y - ChunkCenterY;
	int32 MaxDistance = (RenderDistance + 1) * ChunkSize * CellSize;

	return FMath::Abs(XDiff) > MaxDistance || FMath::Abs(YDiff) > MaxDistance;
}

void AProceduralTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	// Destroying chunks that are far from player
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

	// Rendering chunks nearby
	TArray<FVector2D> StartCoordsOfAdjacentChunks = GetStartCoordsForAdjacentChunks(PlayerLocation);

	for (FVector2D& StartCoords : StartCoordsOfAdjacentChunks)
	{
		if (!RenderedChunks.Contains(StartCoords))
		{
			UChunkProvider* ChunkProvider = NewObject<UChunkProvider>(this);
			if (ChunkProvider)
			{
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
		}
	}
}
