// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Chunk.h"

#include "SingleClassObjectPool.generated.h"

class UPoolClassDataAsset;
class UPoolInterface;
struct FSingleClassPoolData;
class ULevelObjectPool;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class USingleClassObjectPool : public UObject
{
	GENERATED_BODY()

public:
    typedef TDoubleLinkedList<FObjectChunk*> ChunkList;
    //node of list
    typedef ChunkList::TDoubleLinkedListNode ChunkListNode;
public:

    bool Init(UClass* Class, ULevel* level);

    bool Register(UPoolClassDataAsset* PoolClassDataAsset, TSharedPtr<FChunkPool> ChunkPool, uint32 StartPos, ULevelObjectPool* InLevelObjectPool);

    void UnRegister(UPoolClassDataAsset* PoolClassDataAsset);

    /*
    * Get an object from FreeChain to UsedChain
    */
    UObject* AllocateObject();

    /*
    * Free an object, only called by IPoolInterface
    */
    void FreeObject(ChunkListNode* node);

    //get the used count of this pool
    int32 getusedcount() { return UsedChain.Num(); }
    //get the free count of this pool
    int32 getfreecount() { return FreeChain.Num(); }
    //get the all count of this pool
    int32 getallcount() { return AllChain.Num(); }
    //get the ready count of this pool
    int32 getreadycount() { return getusedcount() + getfreecount(); }

    void Tick(float DeltaTime);
private:
    UObject* NewThisObject(uint32 PosInAllChain);
    
    ////allocate a object from this pool
    //tweakptr<uobject> allocateobject();
    ////free a object to this pool
    //void freeobject(tweakptr<uobject> object);
    ////free all object in this pool
    //void freeallobject();
    ////expand the pool
    //void expandpool(uint32 size);
    ////shrink the pool
    //void shrinkpool(uint32 size);
    ////get the class of this pool
    //uclass* getclass() { return objectclass.get(); 
    ////get the used chain of this pool
    //tdoublelinkedlist<tweakptr<objectchunk>>& getusedchain() { return usedchain; }
    ////get the free chain of this pool
    //tdoublelinkedlist<tweakptr<objectchunk>>& getfreechain() { return freechain; }
    ////get the all chain of this pool
    //tdoublelinkedlist<tsharedptr<objectchunk>>& getallchain() { return allchain; }



private:
    FSingleClassPoolData* PoolDataForUI;

    bool InitFlag;

    //all the dataasset that used in this pool
    TArray<TWeakObjectPtr<UPoolClassDataAsset>> PoolClassDataAssetArray;

    TWeakObjectPtr<ULevel> Level;
    TWeakObjectPtr<ULevelObjectPool> LevelObjectPool;

    TSharedPtr<FChunkPool> UsingChunkPool;
    uint32 StartPosInPool;

    TWeakObjectPtr<UClass> ObjectClass;



    //used chain
    ChunkList UsedChain;

    //free chain
    ChunkList FreeChain;

    //AllChunk
    ChunkList AllChain;
};
