// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "ObjectPoolSubsystem.generated.h"

class ULevelObjectPoolDataAsset;
class ULevelObjectPool;
/**
 * 
 */
UCLASS()
class OBJECTPOOLSYSTEM_API UObjectPoolSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
    /*
    * There is 2 ways to create a new AActor
    * 1. rewrite SpawnActor in UWorld to an AActor only create and init but not add to Scene
    * 2. use SpawnActorDeferred to create an AActor, then call FinishSpawningActor to add to Scene
    * It is better to use 1, because 1 can have more control
    */
public:
    //Todo: Spawn Actor as way 1
    UFUNCTION(BlueprintCallable, Category = "Spawning", meta = (WorldContext = "WorldContextObject", DisplayName = "Spawn From Pool", ScriptName = "SpawnActorFromPool"))
    AActor* SpawnActor(const UObject* WorldContextObject
        , UPARAM(meta = (MustImplement = "PoolActorInterface")) TSubclassOf<AActor> ActorClass
        ,const FTransform& SpawnTransform
        , ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined);

    UFUNCTION(BlueprintCallable, Category = "Spawning", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Object From Pool", ScriptName = "GetUObjectFromPool"))
    UObject* GetUObject(const UObject* WorldContextObject, UPARAM(meta = (MustImplement = "PoolInterface")) TSubclassOf<UObject> ObjectClass);
    

    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    ULevelObjectPool* GetLevelObjectPool(ULevel* level);

public:
    // USubsystem implementation Begin
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    // USubsystem implementation End

    //Todo: Make this subsystem tickable

private:
    friend class ULevelObjectPoolDataAsset;
    bool RegisterLevelObjectPool(ULevelObjectPoolDataAsset* LevelObjectPoolDataAsset, ULevel* level);

    void UnRegisterLevelObjectPool(ULevelObjectPoolDataAsset* LevelObjectPoolDataAsset, ULevel* level);
private:

    UPROPERTY()
    TMap<ULevel*, TObjectPtr<ULevelObjectPool>> PoolMap;
};
