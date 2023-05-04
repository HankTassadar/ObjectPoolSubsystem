// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorSpawnPerformanceTest.h"
#include "Macro/ObjectPoolSystemMacro.h"

// Sets default values
AActorSpawnPerformanceTest::AActorSpawnPerformanceTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActorSpawnPerformanceTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActorSpawnPerformanceTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::Tick);
    for (int32 i = 0; i < SpawnCountPerTick; ++i)
    {
        AActor* actor = SpawnActors();
        SpawnedActors.Add(actor);
    }

    for (AActor* actor : SpawnedActors)
    {
        if (actor)
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::Tick_DestroyActor);
            actor->Destroy();
        }
    }

    SpawnedActors.Empty();

    UE_LOG(LogObjectPool, Log, TEXT("Actor Array Size: %d"), GetLevel()->Actors.Num());
}

AActor* AActorSpawnPerformanceTest::SpawnActors()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::SpawnActors);
    FName name = FName(*FString::Printf(TEXT("AActorSpawnPerformanceTest_Actors_%d"), SpawnNameUnique++));
    ULevel* level = GetLevel();
    AActor* actor = nullptr;

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::SpawnActors_NewObject);
        actor = NewObject<AActor>(level, ActorToSpawn, name, EObjectFlags::RF_Transactional);
    }

    if (!actor)
    {
        return nullptr;
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::SpawnActors_AddToLevel);
        level->Actors.Add(actor);
        level->ActorsForGC.Add(actor);
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::SpawnActors_PostSpawnInitialize);
        actor->PostSpawnInitialize(FTransform(), nullptr, nullptr, false, false, true);
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::SpawnActors_CheckDefaultSubobjects);
        actor->CheckDefaultSubobjects();
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::SpawnActors_FinishSpawning);
        actor->FinishSpawning(FTransform());
    }

    return actor;
}

