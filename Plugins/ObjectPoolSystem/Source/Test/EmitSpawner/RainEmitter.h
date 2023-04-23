// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmitSpawnerBase.h"

#include "RainEmitter.generated.h"

/**
 * 
 */
UCLASS()
class OBJECTPOOLSYSTEM_API URainEmitter : public UEmitSpawnerBase
{
	GENERATED_BODY()

public:
    virtual FTransform GenerateSpawnTransform() override;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmitSpawner")
    FVector2D SpawnRange = FVector2D(0, 0);
};
