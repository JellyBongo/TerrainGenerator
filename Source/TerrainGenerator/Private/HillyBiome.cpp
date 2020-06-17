// Fill out your copyright notice in the Description page of Project Settings.


#include "HillyBiome.h"

UHillyBiome::UHillyBiome()
{
	SimplexNoise = NewObject<UFastNoise>(this, "HillySimplexNoiseGenerator");
	SimplexNoise->SetNoiseType(ENoiseType::SimplexFractal);
	SimplexNoise->SetFractalOctaves(5);
	SimplexNoise->SetFractalGain(0.35);
	SimplexNoise->SetFrequency(0.00005);
	SimplexNoise->SetFractalLacunarity(2.3);
	SimplexNoise->SetSeed(Seed);
	SimplexNoise->SetFractalType(EFractalType::FBM);
}

int32 UHillyBiome::GetHeight(int32 X, int32 Y)
{
	return (SimplexNoise->GetNoise(X, Y) + 1) / 2 * MaxHeight;
}
