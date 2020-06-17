// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Biome.generated.h"

/**
 * Base class for all Biome classes
 */
UCLASS()
class TERRAINGENERATOR_API UBiome : public UObject
{
	GENERATED_BODY()

public:
	virtual int32 GetHeight(int32 X, int32 Y);

	UFUNCTION()
	void SetMaxHeight(int32 InMaxHeight);

	UFUNCTION()
	void SetSeed(int32 InSeed);

protected:
	int32 MaxHeight = 1000;
	int32 Seed = 1337;
};
