// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "PoolClassDataAsset.generated.h"

class ULevel;
/**
 * 
 */
UCLASS()
class OBJECTPOOLSYSTEM_API UPoolClassDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

public:
    // The class of the object using in pool
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoolClassDataAsset",meta = (MustImplement = "PoolInterface"))
    TSubclassOf<UObject> Class;
    
    // The number of objects to preallocate in the pool
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoolClassDataAsset")
    int32 PreallocateCount = 0;
    
    //Todo: soft regist to level, no block level load
    
    //Todo: Auto Expand Pool
    //// If true, the pool will expand if it runs out of objects
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoolClassDataAsset")
    //bool bCouldExpand = true;

    ////when reach this percentage of poolsize, will expand, 0~100
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoolClassDataAsset")
    //int32 ExpandPercentage = FMath::Clamp(50, 0, 100);

    ///*
    // * expand max count pre tick limit
    // * if ExpandTimeLimitPerTick is not 0 and expand time used per tick is over it, will not expand more
    // */
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoolClassDataAsset")
    //int32 ExpandCountPerTick = 1;

    ///*
    // * if expand count automatically decided by frame, will make expand used time per tick smaller than this value
    // *  ms
    // * if 0, will expand ExpandCountPerTick in every tick
    // */
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoolClassDataAsset")
    //int32 ExpandTimeLimitPerTick = 0;
    
    
};
