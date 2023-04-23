// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PoolClassDataAsset.h"
#include "LevelObjectPoolDataAsset.generated.h"

class ULevel;
/**
 * 
 */
UCLASS()
class OBJECTPOOLSYSTEM_API ULevelObjectPoolDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
    bool RegisterToObjectPoolSubsystem(ULevel* level);

    UFUNCTION(BlueprintCallable, Category = "ObjectPool", meta = (Keywords = "Regist", DisplayName = "Regist To Pool", ScriptName = "RegistToPool"))
    bool K2_RegisterToObjectPoolSubsystem(ULevel* level);

    void UnRegisterFromObjectPoolSubsystem(ULevel* level);

    UFUNCTION(BlueprintCallable, Category = "ObjectPool", meta = (Keywords = "UnRegist", DisplayName = "UnRegist From Pool", ScriptName = "UnRegistFromPool"))
    void K2_UnRegisterFromObjectPoolSubsystem(ULevel* level);

public:
    /* 
     * The level to pool objects for
     * when a level try to get object from pool, if mutiple levels have the same poolclass, it will get from self's pool
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pool")
    TArray<TObjectPtr<UPoolClassDataAsset>> PoolClassArray;

};
