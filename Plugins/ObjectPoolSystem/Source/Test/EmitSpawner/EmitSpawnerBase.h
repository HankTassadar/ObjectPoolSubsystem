// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "EmitSpawnerBase.generated.h"

/**
 * 
 */
UCLASS()
class OBJECTPOOLSYSTEM_API UEmitSpawnerBase : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    virtual FTransform GenerateSpawnTransform() {return FTransform();}

    int32 GetSpawnCountPerTick() const {return SpawnCountPerTick;}

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmitSpawner")
    int32 SpawnCountPerTick = 0;

    //if 0, will spawn every tick
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmitSpawner")
    float SpawnInterval = 0;
};
