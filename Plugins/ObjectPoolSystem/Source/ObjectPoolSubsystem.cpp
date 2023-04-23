// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPoolSubsystem.h"
#include "Macro/ObjectPoolSystemMacro.h"
#include "DataAsset/LevelObjectPoolDataAsset.h"
#include "PoolInterface/PoolActorInterface.h"
#include "ObjectPool/LevelObjectPool.h"

AActor* UObjectPoolSubsystem::SpawnActor(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride)
{
    if (!WorldContextObject)
    {
        UE_LOG(LogObjectPool, Error, TEXT("WorldContextObject is null"));
        return nullptr;
    }

    ULevel* level = WorldContextObject->GetTypedOuter<ULevel>();
    if (!level) //make sure has an level to spawn the new actor in
    {
        UE_LOG(LogObjectPool, Error, TEXT("WorldContextObject is not in a level"));
        return nullptr;
    }

    if (!ActorClass.Get())
    {
        UE_LOG(LogObjectPool, Error, TEXT("ActorClass is null"));
        return nullptr;
    }

    if (!PoolMap.Contains(level))
    {
        UE_LOG(LogObjectPool, Error, TEXT("level pool is not registed"));
        return nullptr;
    }

    ULevelObjectPool* pool = PoolMap[level];
    if (!pool)
    {
        UE_LOG(LogObjectPool, Error, TEXT("level pool is null"));
        return nullptr;
    }

    UObject* object = pool->AllocateObject(ActorClass);

    if (nullptr == object)
    {
        //UE_LOG(LogObjectPool, Error, TEXT("AllocateObject return null"));
        return nullptr;
    }

    AActor* actor = Cast<AActor>(object);
    if (!actor)
    {
        UE_LOG(LogObjectPool, Error, TEXT("Cast Failed!"));
        return nullptr;
    }

    IPoolActorInterface* poolactor = Cast<IPoolActorInterface>(actor);
    if (!poolactor)
    {
        UE_LOG(LogObjectPool, Error, TEXT("AllocateObject return null"));
        return nullptr;
    }

    bool flag = poolactor->AttachToLevel(level, SpawnTransform, CollisionHandlingOverride);
    if (!flag)
    {
        UE_LOG(LogObjectPool, Error, TEXT("AttachToLevel failed"));
        poolactor->ReleaseToPool();
        return nullptr;
    }

    return actor;
}

UObject* UObjectPoolSubsystem::GetUObject(const UObject* WorldContextObject, TSubclassOf<UObject> ObjectClass)
{
    check(WorldContextObject);

    if (!ObjectClass.Get())
    {
        UE_LOG(LogObjectPool, Error, TEXT("ObjectClass is null"));
        return nullptr;
    }
    
    ULevel* level = WorldContextObject->GetTypedOuter<ULevel>();
    if (!level) //make sure has an level to spawn the new actor in
    {
        UE_LOG(LogObjectPool, Error, TEXT("WorldContextObject is not in a level"));
        return nullptr;
    }

    if (!PoolMap.Contains(level))
    {
        UE_LOG(LogObjectPool, Error, TEXT("level pool is not registed"));
        return nullptr;
    }

    ULevelObjectPool* pool = PoolMap[level];
    if (!pool)
    {
        UE_LOG(LogObjectPool, Error, TEXT("level pool is null"));
        return nullptr;
    }

    return pool->AllocateObject(ObjectClass);
}

ULevelObjectPool* UObjectPoolSubsystem::GetLevelObjectPool(ULevel* level)
{
    if (!level)
    {
        UE_LOG(LogObjectPool, Error, TEXT("level is null"));
        return nullptr;
    }

    if (!PoolMap.Contains(level))
    {
        UE_LOG(LogObjectPool, Error, TEXT("this level is not registed, level: [%s]"), *level->GetName());
        return nullptr;
    }

    ULevelObjectPool* levelpool = PoolMap[level];
    if (!levelpool)
    {
        UE_LOG(LogObjectPool, Error, TEXT("levelpool is null"));
        return nullptr;
    }

    UE_LOG(LogObjectPool, Log, TEXT("GetLevelObjectPool, level [%s], levelobjectpool [%s]")
        , *level->GetName(), *levelpool->GetName());

    return levelpool;
}

void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogObjectPool, Log, TEXT("ObjectPoolSubsystem Initialize"));
}

void UObjectPoolSubsystem::Deinitialize()
{
    Super::Deinitialize();
    UE_LOG(LogObjectPool, Log, TEXT("ObjectPoolSubsystem Deinitialize"));
}

bool UObjectPoolSubsystem::RegisterLevelObjectPool(ULevelObjectPoolDataAsset* LevelObjectPoolDataAsset, ULevel* level)
{
    if (!LevelObjectPoolDataAsset)
    {
        UE_LOG(LogObjectPool, Error, TEXT("LevelObjectPoolDataAsset is null"));
        return false;
    }
    if (!level)
    {
        UE_LOG(LogObjectPool, Error, TEXT("level is null"));
        return false;
    }

    TObjectPtr<ULevelObjectPool> levelpool;
    if (!PoolMap.Contains(level))
    {
        //new a levelpool
        levelpool = NewObject<ULevelObjectPool>(this);
        levelpool->Init(level);
        PoolMap.Add(level, levelpool);
    }

    bool succeed_state = levelpool->RegisterLevelObjectPool(LevelObjectPoolDataAsset);
    if (!succeed_state)
    {
        levelpool->UnRegisterLevelObjectPool(LevelObjectPoolDataAsset);
        UE_LOG(LogObjectPool, Error, TEXT("RegistLevelObjectPool failed"))
    }

    UE_LOG(LogObjectPool, Log, TEXT("RegistLevelObjectPool, LevelObjectPoolDataAsset [%s], level [%s], levelpool [%s]"),
               *LevelObjectPoolDataAsset->GetName(), *level->GetName(), *levelpool->GetName());
    return succeed_state;
}

void UObjectPoolSubsystem::UnRegisterLevelObjectPool(ULevelObjectPoolDataAsset* LevelObjectPoolDataAsset, ULevel* level)
{
    if (!LevelObjectPoolDataAsset)
    {
        UE_LOG(LogObjectPool, Error, TEXT("LevelObjectPoolDataAsset is null"));
        return;
    }
    if (!level)
    {
        UE_LOG(LogObjectPool, Error, TEXT("level is null"));
        return;
    }
    
    if (!PoolMap.Contains(level))
    {
        UE_LOG(LogObjectPool, Error, TEXT("this level [%s] has not registed any levelpool"), *level->GetName());
        return;
    }

    ULevelObjectPool* levelpool = PoolMap[level];
    if (!levelpool)
    {
        UE_LOG(LogObjectPool, Error, TEXT("levelpool is null"));
        return;
    }

    levelpool->UnRegisterLevelObjectPool(LevelObjectPoolDataAsset);

    UE_LOG(LogObjectPool, Log, TEXT("UnRegistLevelObjectPool, LevelObjectPoolDataAsset [%s], level [%s], levelpool [%s]"),
               *LevelObjectPoolDataAsset->GetName(), *level->GetName(), *levelpool->GetName());
    return;
}
