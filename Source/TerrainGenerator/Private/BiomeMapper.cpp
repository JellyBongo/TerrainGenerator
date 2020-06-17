// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomeMapper.h"
#include "HillyBiome.h"
#include "PlainBiome.h"

UBiomeMapper::UBiomeMapper()
{
	InitializeBiomes();
	Noise = NewObject<UFastNoise>(this, "MapperNoise");
}

void UBiomeMapper::SetSeed(int32 InSeed)
{
	Seed = InSeed;
	Noise->SetSeed(InSeed);
}

void UBiomeMapper::SetMaxHeight(int32 InMaxHeight)
{
	MaxHeight = InMaxHeight;
	InitializeBiomes();
}

UBiome* UBiomeMapper::GetBiomeAtPoint(int32 X, int32 Y)
{
	return Biomes[0];
}

void UBiomeMapper::InitializeBiomes()
{
	UHillyBiome* Biome = NewObject<UHillyBiome>(this, "HillyBiome");
	Biome->SetMaxHeight(MaxHeight);
	Biome->SetSeed(Seed);
	Biomes.Add(Biome);
}
