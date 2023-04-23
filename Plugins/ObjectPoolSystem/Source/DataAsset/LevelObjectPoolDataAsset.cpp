// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelObjectPoolDataAsset.h"
#include "../ObjectPoolSubsystem.h"
#include "Macro/ObjectPoolSystemMacro.h"
#include "../PoolInterface/PoolInterface.h"

bool ULevelObjectPoolDataAsset::RegisterToObjectPoolSubsystem(ULevel* level)
{
    if (!level)
    {
        UE_LOG(LogObjectPool, Error, TEXT("ULevel is null"));
        return false;
    }

    //check if PoolClassArray has the same Class in it, if has, drop the later one
    TSet<UClass*> ClassSet;
    TArray<TObjectPtr<UPoolClassDataAsset>> NewPoolClassArray;
    bool bHasSameClass = false;
    for (auto& ClassDataAsset : PoolClassArray)
    {
        if (!ClassDataAsset->Class.Get())
        {
            UE_LOG(LogObjectPool, Error, TEXT("ActorClass in PoolClassDataAsset [%s] in LevelObjectPoolDataAsset [%s] is null")
                , *ClassDataAsset->GetName(), *GetName());
            return false;
        }
        if (ClassSet.Contains(ClassDataAsset->Class.Get()))
        {
            UE_LOG(LogObjectPool, Warning
                , TEXT("ActorClass [%s] in PoolClassDataAsset [%s] in LevelObjectPoolDataAsset [%s] is the same as the previous one")
                , *ClassDataAsset->GetName(), *GetName());
            bHasSameClass = true;
        }
        else
        {
            //check if the ActorClass implements PoolInterface
            if (!ClassDataAsset->Class->ImplementsInterface(UPoolInterface::StaticClass()))
            {
                UE_LOG(LogObjectPool, Error
                    , TEXT("ActorClass in PoolClassDataAsset [%s] in LevelObjectPoolDataAsset [%s] is not a sub class of PoolInterface")
                    , *ClassDataAsset->GetName(), *GetName());
                return false;
            }

            ClassSet.Add(ClassDataAsset->Class.Get());
            NewPoolClassArray.Add(ClassDataAsset);
        }
    }
    if (bHasSameClass)
    {
        this->PoolClassArray = NewPoolClassArray;
    }
    
    UWorld* World = level->GetWorld();
    if (nullptr == World)
    {
        UE_LOG(LogObjectPool, Error, TEXT("UWorld is null, the level is not in a World"));
        return false;
    }

    UObjectPoolSubsystem* ObjectPoolSubsystem = World->GetSubsystem<UObjectPoolSubsystem>();
    if (ObjectPoolSubsystem)
    {
        return ObjectPoolSubsystem->RegisterLevelObjectPool(this, level);
    }
    else
    {
        UE_LOG(LogObjectPool, Error, TEXT("ObjectPoolSubsystem is null"));
    }

    return false;
}

bool ULevelObjectPoolDataAsset::K2_RegisterToObjectPoolSubsystem(ULevel* level)
{
    return RegisterToObjectPoolSubsystem(level);
}

void ULevelObjectPoolDataAsset::UnRegisterFromObjectPoolSubsystem(ULevel* level)
{
    if (!level)
    {
        UE_LOG(LogObjectPool, Error, TEXT("ULevel is null"));
        return;
    }
    UWorld* World = level->GetWorld();
    if (nullptr == World)
    {
        UE_LOG(LogObjectPool, Error, TEXT("UWorld is null, the level is not in a World"));
        return;
    }
    UObjectPoolSubsystem* ObjectPoolSubsystem = World->GetSubsystem<UObjectPoolSubsystem>();
    if (ObjectPoolSubsystem)
    {
        ObjectPoolSubsystem->UnRegisterLevelObjectPool(this, level);
    }
    else
    {
        UE_LOG(LogObjectPool, Error, TEXT("ObjectPoolSubsystem is null"));
    }
}

void ULevelObjectPoolDataAsset::K2_UnRegisterFromObjectPoolSubsystem(ULevel* level)
{
    UnRegisterFromObjectPoolSubsystem(level);
}
