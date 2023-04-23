// Fill out your copyright notice in the Description page of Project Settings.


#include "EmitSpawnerActor.h"
#include "../Private/Macro/ObjectPoolSystemMacro.h"
#include "../ObjectPoolSubsystem.h"

// Sets default values
AEmitSpawnerActor::AEmitSpawnerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEmitSpawnerActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEmitSpawnerActor::Tick(float DeltaTime)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(AEmitSpawnerActor::Tick);
	Super::Tick(DeltaTime);
    if (!Emitter)
    {
        return;
    }

    if (Emitter->SpawnInterval > 0)
    {
        if (SpawnTimer < Emitter->SpawnInterval)
        {
            SpawnTimer += DeltaTime;

            return;

        }
        else
        {
            SpawnTimer -= Emitter->SpawnInterval;
        }
    };

    FTransform transform;
    int32 spawnCount = Emitter->GetSpawnCountPerTick();

    for (int32 i = 0; i < spawnCount; ++i)
    {
        // Spawn
        transform = Emitter->GenerateSpawnTransform() + this->GetTransform();
        if (bUseObjectPool)
        {
            SpawnObjectPool(transform);
        }
        else
        {
            SpawnNormal(transform);
        }
    }
}

void AEmitSpawnerActor::SpawnNormal(FTransform& SpawnTransform)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(AEmitSpawnerActor::SpawnNormal);
    UWorld* world = GetWorld();
    check(world);
    world->SpawnActor<AActor>(NormalSpawnActorClass, SpawnTransform);
}

void AEmitSpawnerActor::SpawnObjectPool(FTransform& SpawnTransform)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(AEmitSpawnerActor::SpawnObjectPool);
    UWorld* world = GetWorld();
    check(world);
    UObjectPoolSubsystem* poolSubsystem = world->GetSubsystem<UObjectPoolSubsystem>();
    check(poolSubsystem);
    poolSubsystem->SpawnActor(this, ObjectPoolSpawnActorClass, SpawnTransform);
}

