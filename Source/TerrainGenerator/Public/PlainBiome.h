// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Biome.h"
#include "FastNoise.h"
#include "PlainBiome.generated.h"

/**
 * Biome generating almost flat plains
 */
UCLASS()
class TERRAINGENERATOR_API UPlainBiome : public UBiome
{
	GENERATED_BODY()
	
public:
	UPlainBiome();

	UFUNCTION()
	int32 GetHeight(int32 X, int32 Y) override;

private:
	UFastNoise* Noise;
};
