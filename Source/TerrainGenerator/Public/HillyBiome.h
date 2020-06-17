// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Biome.h"
#include "FastNoise.h"
#include "HillyBiome.generated.h"

/**
 * Biome generating hilly landscape
 */
UCLASS()
class TERRAINGENERATOR_API UHillyBiome : public UBiome
{
	GENERATED_BODY()
	
public:
	UHillyBiome();

	UFUNCTION()
	int32 GetHeight(int32 X, int32 Y) override;

private:
	UFastNoise* SimplexNoise;
};
