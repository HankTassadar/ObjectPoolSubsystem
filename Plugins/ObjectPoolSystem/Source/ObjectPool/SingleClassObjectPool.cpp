// Fill out your copyright notice in the Description page of Project Settings.


#include "SingleClassObjectPool.h"
#include "Macro/ObjectPoolSystemMacro.h"
#include "../DataAsset/PoolClassDataAsset.h"
#include "../PoolInterface/PoolInterface.h"
#include "LevelObjectPool.h"

bool USingleClassObjectPool::Init(UClass* Class, ULevel* level)
{
    if (nullptr == Class)
    {
        UE_LOG(LogObjectPool, Error, TEXT("Class is null"));
        return false;
    }
    ObjectClass = Class;
    Level = level;
    InitFlag = true;

    return true;
}

bool USingleClassObjectPool::Register(UPoolClassDataAsset* PoolClassDataAsset, TSharedPtr<FChunkPool> ChunkPool, uint32 StartPos, ULevelObjectPool* InLevelObjectPool)
{
    LevelObjectPool = InLevelObjectPool;
    if (nullptr == PoolClassDataAsset)
    {
        UE_LOG(LogObjectPool, Error, TEXT("PoolClassDataAsset is null"));
        return false;
    }

    //do not register twice
    if (PoolClassDataAssetArray.Contains(PoolClassDataAsset))
    {
        UE_LOG(LogObjectPool, Warning, TEXT("PoolClassDataAsset is already registered, no need to registed twice"));
        return true;
    }

    this->UsingChunkPool = ChunkPool;
    this->StartPosInPool = StartPos;

    //put these chunk to allchain and freechain
    for (int32 i = 0; i < PoolClassDataAsset->PreallocateCount; ++i)
    {
        //AllChain.Add(UsingChunkPool->GetChunk(StartPosInPool + i));
        AllChain.AddTail(&UsingChunkPool->ChunkArray[StartPosInPool + i]);
        FreeChain.AddTail(&UsingChunkPool->ChunkArray[StartPosInPool + i]);
    }

    //init these object in allchain
    uint32 PosInAllChain = 0;
    //Traversal all the node in allchain
    for (ChunkListNode* Node = FreeChain.GetHead(); Node; Node = Node->GetNextNode())
    {
        FObjectChunk* Chunk = Node->GetValue();

        if (nullptr != Chunk->Object)
        {
            UE_LOG(LogObjectPool, Warning, TEXT("Chunk->Object is valid before init"));
            continue;
        }

        //real new object
        Chunk->Object = NewThisObject(PosInAllChain++);

        if (nullptr != Chunk->Object)
        {
            //this object should not be garbage collected,until it is released to pool and unregistered
            //Log info
            //UE_LOG(LogObjectPool, Log, TEXT("New Object succeed, ClassName: [%s], ObjectName: [%s]"), *ObjectClass->GetName(), *Chunk->Object->GetName());
            IPoolInterface* PoolInterface = CastChecked<IPoolInterface>(Chunk->Object);
            if (PoolInterface)
            {
                PoolInterface->OnNewObject();
                PoolInterface->pool = this;
                PoolInterface->node = Node;
            }
            Chunk->Object->AddToRoot();
        }
        else
        {
            UE_LOG(LogObjectPool, Error, TEXT("Chunk->Object is invalid after init, PosInAllChain:[%d]"), PosInAllChain);
        }
    }

    return true;
}

void USingleClassObjectPool::UnRegister(UPoolClassDataAsset* PoolClassDataAsset)
{
    if (nullptr == PoolClassDataAsset)
    {
        UE_LOG(LogObjectPool, Error, TEXT("PoolClassDataAsset is null"));
        return;
    }
    //Log Info
    UE_LOG(LogObjectPool, Log, TEXT("Start UnRegister SingleClassObjectPool, ClassName: [%s]"), *ObjectClass->GetName());

    //Traversal all the object in usedchain
    for (FObjectChunk* Chunk : UsedChain)
    {
        //if the object in usedchain is valid, release it to pool
        if (nullptr != Chunk->Object)
        {
            IPoolInterface* PoolInterface = CastChecked<IPoolInterface>(Chunk->Object);
            if (PoolInterface)
            {
                PoolInterface->ReleaseToPool();
            }
        }
    }
    UsedChain.Empty();

    //Traversal all the object in allchain
    for (FObjectChunk* Chunk : AllChain)
    {
        //if the object in allchain is valid, release it to pool
        if (nullptr != Chunk->Object)
        {
            //Log info
            //UE_LOG(LogObjectPool, Log, TEXT("Release Object, ClassName: [%s], ObjectName: [%s]", *ObjectClass->GetName(), *Chunk->Object->GetName());
            //let the object can be GC
            Chunk->Object->RemoveFromRoot();
            Chunk->Object->MarkAsGarbage();
            IPoolInterface* PoolInterface = CastChecked<IPoolInterface>(Chunk->Object);
            PoolInterface->OnObjectDestroy();
            Chunk->ReSet();
        }
    }
    //clear allchain
    AllChain.Empty();

    //Reset this
    this->UsingChunkPool.Reset();
    this->StartPosInPool = 0;

    //Log Info
    UE_LOG(LogObjectPool, Log, TEXT("End UnRegister SingleClassObjectPool, ClassName: [%s]"), *ObjectClass->GetName());
}

UObject* USingleClassObjectPool::AllocateObject()
{
    //Log Info
    //UE_LOG(LogObjectPool, Log, TEXT("AllocateObject An Object in SingleClassObjectPool, ClassName: [%s]"), *ObjectClass->GetName());

    //Get FreeChain Head
    ChunkListNode* FreeChainHead = FreeChain.GetHead();
    if (nullptr == FreeChainHead)
    {
        //UE_LOG(LogObjectPool, Error, TEXT("FreeChainHead is null"));
        return nullptr;
    }

    //Remove FreeChain Head
    FreeChain.RemoveNode(FreeChainHead, false);
    ChunkListNode* TargetNode = FreeChainHead;

    //Add FreeChain Head to UsedChain
    UsedChain.AddTail(TargetNode);

    //Get the object in FreeChain Head
    UObject* object = TargetNode->GetValue()? FreeChainHead->GetValue()->Object : nullptr;
    if (nullptr == object)
    {
        UE_LOG(LogObjectPool, Error, TEXT("object is null"));
        return nullptr;
    }

    TargetNode->GetValue()->bIsAllocated = true;
    
    return object;
}

void USingleClassObjectPool::FreeObject(ChunkListNode* node)
{
    if (nullptr == node)
    {
        UE_LOG(LogObjectPool, Error, TEXT("node is null when free an Object"));
        return;
    }

    if (!node->GetValue()->bIsAllocated)
    {
        //in case of double free
        return;
    }

    //Log Info
    //UE_LOG(LogObjectPool, Log, TEXT("FreeObject An Object to SingleClassObjectPool, ClassName: [%s]"), *ObjectClass->GetName());

    //Remove UsedChain Node
    UsedChain.RemoveNode(node, false);
    //Add UsedChain Node to FreeChain
    FreeChain.AddTail(node);
    //Get the object in UsedChain Node
    UObject* object = node->GetValue() ? node->GetValue()->Object : nullptr;
    if (nullptr == object)
    {
        UE_LOG(LogObjectPool, Error, TEXT("object is null"));
        return;
    }
    node->GetValue()->bIsAllocated = false;
}

void USingleClassObjectPool::Tick(float DeltaTime)
{
    //Todo: tick pool to expand or shrink, or soft load
}

UObject* USingleClassObjectPool::NewThisObject(uint32 PosInAllChain)
{
    //Log info
    //UE_LOG(LogObjectPool, Log, TEXT("NewThisObject in SingleClassObjectPool, ClassName: [%s]"), *ObjectClass->GetName());

    FStaticConstructObjectParameters Params(ObjectClass.Get());
    Params.Outer = Level.Get();
    //PosInAllChain to string
    Params.Name = FName(ObjectClass->GetName() + "_" + this->GetName() + "_" + FString::FromInt(PosInAllChain));
    Params.SetFlags = EObjectFlags::RF_Transactional;
    Params.Template = nullptr;
    Params.bCopyTransientsFromClassDefaults = false;
    Params.InstanceGraph = nullptr;
    Params.ExternalPackage = nullptr;
    return StaticConstructObject_Internal(Params);
}
