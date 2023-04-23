// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EmitSpawnerBase.h"

#include "EmitSpawnerActor.generated.h"

UCLASS()
class OBJECTPOOLSYSTEM_API AEmitSpawnerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEmitSpawnerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmitSpawner")
    TObjectPtr<UEmitSpawnerBase> Emitter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmitSpawner")
    TSubclassOf<AActor> NormalSpawnActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmitSpawner", Meta = (MustImplement = "PoolActorInterface"))
    TSubclassOf<AActor> ObjectPoolSpawnActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EmitSpawner")
    bool bUseObjectPool = false;
private:
    void SpawnNormal(FTransform& SpawnTransform);
    void SpawnObjectPool(FTransform& SpawnTransform);

    float SpawnTimer = 0.f;
};
