// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Biome.h"
#include "FastNoise.h"
#include "UObject/NoExportTypes.h"
#include "BiomeMapper.generated.h"

/**
 * Class used to map each point of the world to some biome
 */
UCLASS()
class TERRAINGENERATOR_API UBiomeMapper : public UObject
{
	GENERATED_BODY()

private:
	TArray<UBiome*> Biomes;
	UFastNoise* Noise;
	int32 Seed = 1337;
	int32 MaxHeight = 1500;

public:
	UBiomeMapper();

	void SetSeed(int32 InSeed);
	void SetMaxHeight(int32 InMaxHeight);

	UBiome* GetBiomeAtPoint(int32 X, int32 Y);

private:
	void InitializeBiomes();

};
