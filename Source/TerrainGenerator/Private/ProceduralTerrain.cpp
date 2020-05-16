// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralTerrain.h"
#include "ChunkProvider.h"
#include "Providers/RuntimeMeshProviderBox.h"
#include "Providers/RuntimeMeshProviderCollision.h"
//#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

AProceduralTerrain::AProceduralTerrain() : Material(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AProceduralTerrain::BeginPlay()
{
	Super::BeginPlay();

	//UExampleTerrainProvider* TerrainProvider = NewObject<UExampleTerrainProvider>(this, TEXT("TerrainProvider"));

	//URuntimeMeshProviderCollision* CollisionProvider = NewObject<URuntimeMeshProviderCollision>(this, TEXT("CollisionProvider"));
	//if (TerrainProvider)
	//{
	//	TerrainProvider->SetDisplayMaterial(Material);
	//	/*CollisionProvider->SetChildProvider(TerrainProvider);
	//	CollisionProvider->SetRenderableLODForCollision(1);
	//	CollisionProvider->SetRenderableSectionAffectsCollision(0, true);
	//	FRuntimeMeshCollisionSettings CollisionSettings;
	//	CollisionSettings.bUseComplexAsSimple = true;
	//	CollisionSettings.bUseAsyncCooking = true;
	//	CollisionProvider->SetCollisionSettings(CollisionSettings);*/
	//	GetRuntimeMeshComponent()->Initialize(TerrainProvider);
	//}

}

int32 FloorByGrid(float Location, int32 Grid)
{
	int SnappedLocation = FMath::GridSnap(Location, Grid);
	return SnappedLocation > Location ? (SnappedLocation - Grid) : SnappedLocation;
}

TArray<FVector2D> AProceduralTerrain::GetStartCoordsForAdjacentChunks(const FVector& PlayerLocation) const
{
	TArray<FVector2D> StartCoords;
	int32 XFloored = FloorByGrid(PlayerLocation.X, ChunkSize * CellSize);
	int32 YFloored = FloorByGrid(PlayerLocation.Y, ChunkSize * CellSize);

	for (int32 X = XFloored - ChunkSize * RenderDistance; X < XFloored + ChunkSize * (RenderDistance + 1); X += ChunkSize)
	{
		for (int32 Y = YFloored - ChunkSize * RenderDistance; Y < YFloored + ChunkSize * (RenderDistance + 1); Y += ChunkSize)
		{
			StartCoords.Add(FVector2D(X, Y));
		}
	}

	return StartCoords;
}

void AProceduralTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	TArray<FVector2D> StartCoordsOfAdjacentChunks = GetStartCoordsForAdjacentChunks(PlayerLocation);

	/*FVector2D StartCoords = StartCoordsOfAdjacentChunks[0];
	if (!RenderedChunks.Contains(StartCoords))
	{
		UChunkProvider* ChunkProvider = NewObject<UChunkProvider>(this, TEXT("ChunkProvider"));
		if (ChunkProvider)
		{
			ChunkProvider->SetDisplayMaterial(Material);
			ChunkProvider->SetChunkSize(ChunkSize);
			ChunkProvider->SetCellSize(CellSize);
			ChunkProvider->SetMaxHeight(MaxHeight);
			ChunkProvider->SetStartCoords(StartCoords);
			ChunkProvider->CalculateBounds();
			GetRuntimeMeshComponent()->Initialize(ChunkProvider);
		}
		RenderedChunks.Add(StartCoords);
	}*/
	/*for (FVector2D StartCoords : StartCoordsOfAdjacentChunks)*/
	for(int32 i = 0; i < 1; i++)
	{
		FVector2D StartCoords = StartCoordsOfAdjacentChunks[i];
		if (!RenderedChunks.Contains(StartCoords))
		{
 			UChunkProvider* ChunkProvider = NewObject<UChunkProvider>(this, TEXT("ChunkProvider"));
			if (ChunkProvider)
			{
				ChunkProvider->SetDisplayMaterial(Material);
				ChunkProvider->SetChunkSize(ChunkSize);
				ChunkProvider->SetCellSize(CellSize);
				ChunkProvider->SetMaxHeight(MaxHeight);
				ChunkProvider->SetStartCoords(StartCoords);
				ChunkProvider->CalculateBounds();
				//ARuntimeMeshActor* ChunkMesh = NewObject<ARuntimeMeshActor>(this, TEXT("ChunkMesh"));
				//ChunkMesh->GetRuntimeMeshComponent()->Initialize(ChunkProvider);
			}
			RenderedChunks.Add(StartCoords);
		}
	}

}
