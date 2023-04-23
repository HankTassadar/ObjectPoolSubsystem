// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Chunk.h"
#include "LevelObjectPoolUI.h"

#include "LevelObjectPool.generated.h"


class ULevelObjectPoolDataAsset;
class USingleClassObjectPool;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ULevelObjectPool : public UObject
{
	GENERATED_BODY()
public:
    typedef TArray<TSharedPtr<FChunkPool>> ChunkPoolArray;
    typedef TMap<TWeakObjectPtr<ULevelObjectPoolDataAsset>, ChunkPoolArray> ChunkPoolMap;

public:
    bool Init(ULevel* level) { Level = level; return true; };

    /*
    * regist a level object pool data asset
    */
    bool RegisterLevelObjectPool(ULevelObjectPoolDataAsset* LevelObjectPoolDataAsset);

    /*
    * reverse regist a level object pool data asset, usually called when the level is destroyed
    * When Regist failed, should call this function to clear the class already regist
    */
    void UnRegisterLevelObjectPool(ULevelObjectPoolDataAsset* LevelObjectPoolDataAsset);

    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    UUserWidget* SetLevelObjectPoolUIByClass(TSubclassOf<ULevelObjectPoolUI> uiClass);

    UFUNCTION(BlueprintCallable, Category = "ObjectPool")
    void SetLevelObjectPoolUI(ULevelObjectPoolUI* ui);

    UObject* AllocateObject(UClass* Class);

    void TickPool(float DeltaTime);

private:
    void UpdateData();
private:
    TWeakObjectPtr<ULevel> Level;

    /*
    * every ChunkPoolArray[0] is the preallocated
    * one ULevelObjectPoolDataAsset can have only one ChunkPoolArray in this level
    */
    ChunkPoolMap Map_ChunkPool;
public:
    //all class can allocte in the levelobjectpool
    UPROPERTY(BlueprintReadWrite,Category = "ObjectPool")
    TMap<UClass*, TObjectPtr<USingleClassObjectPool>> Map_ClassPool;

    UPROPERTY(BlueprintReadWrite, Category = "ObjectPool")
    TObjectPtr<ULevelObjectPoolUI> LevelObjectPoolUI;
};
