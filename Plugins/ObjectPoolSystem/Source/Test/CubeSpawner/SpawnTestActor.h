// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SpawnTestUI.h"
#include "SpawnTestActor.generated.h"

/*
* For testing the spawn and despawn in normal spawn way and object pool way
*/
UCLASS()
class OBJECTPOOLSYSTEM_API ASpawnTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintNativeEvent, Category = "SpawnTestActor")
    void BlueprintSpawnNormal();

    virtual void BlueprintSpawnNormal_Implementation();

    UFUNCTION(BlueprintNativeEvent, Category = "SpawnTestActor")
    void BlueprintSpawnObjectPool();

    virtual void BlueprintSpawnObjectPool_Implementation();
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnTestActor")
    TSubclassOf<USpawnTestUI> SpawnTestUIClass;

    UPROPERTY(BlueprintReadOnly, Category = "SpawnTestActor")
    TObjectPtr<USpawnTestUI> SpawnTestUIInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnTestActor", meta = (MustImplement = "PoolActorInterface"))
    TSubclassOf<AActor> ObjectPoolSpawnClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnTestActor")
    TSubclassOf<AActor> NormalSpawnClass;

private:
    void TickSpawn();

    void SpawnNormal();

    void SpawnObjectPool();

    //will auto destroy actor when actor num over StayCount
    void StayCountAutoDestroy();

    uint32 GetActorCount();

    void DestroyOneActor();

public:
    //the interval between two actor spawn location
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnTestActor")
    FVector SpawnLocationInterval = FVector(100, 100, 100);

    //actor numbers of x,y,z, total actor max number = x*y*z
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnTestActor")
    FVector SpawnLocationCount = FVector(100, 100, 100);

    UFUNCTION(BlueprintCallable, Category = "SpawnTestActor")
    FVector GetSpawnLocation();

    UFUNCTION(BlueprintCallable, Category = "SpawnTestActor")
    void AddToList(AActor* actor);
private:
    //Used by spawn actor, insert in tail and remove from head
    TDoubleLinkedList<TObjectPtr<AActor>> ActorList;
    
    //current spawn count in SpawnLocationCount, 0,0,0 means the next location to spawn is SpawnLocationCount's 0,0,0
    FVector SpawnPosition = FVector(0, 0, 0);

private:
    //Data From SpawnTestUI
    FSpawnTestUIData* UIData = nullptr;
};
