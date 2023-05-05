// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorSpawnPerformanceTest.generated.h"

UCLASS()
class OBJECTPOOLSYSTEM_API AActorSpawnPerformanceTest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorSpawnPerformanceTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Test")
    TSubclassOf<AActor> ActorToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Test")
    int32 SpawnCountPerTick = 100;

private:
    AActor* SpawnActors();

    void TestFinishSpawning(AActor* actor);

    int32 SpawnNameUnique = 0;

    UPROPERTY()
    TSet<AActor*> SpawnedActors;
};
