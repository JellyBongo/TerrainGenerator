// Fill out your copyright notice in the Description page of Project Settings.


#include "PlainBiome.h"

UPlainBiome::UPlainBiome()
{
	Noise = NewObject<UFastNoise>(this, "PlainNoiseGenerator");
	Noise->SetNoiseType(ENoiseType::Simplex);
	Noise->SetFrequency(0.00002);
	Noise->SetSeed(Seed);
	SetMaxHeight(500);
}

int32 UPlainBiome::GetHeight(int32 X, int32 Y)
{
	return (Noise->GetNoise(X, Y) + 1) * MaxHeight / 2;
}