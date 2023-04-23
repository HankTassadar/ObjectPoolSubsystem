// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelObjectPool.h"
#include "Macro/ObjectPoolSystemMacro.h"
#include "../DataAsset/LevelObjectPoolDataAsset.h"
#include "SingleClassObjectPool.h"

bool ULevelObjectPool::RegisterLevelObjectPool(ULevelObjectPoolDataAsset* LevelObjectPoolDataAsset)
{
    if (!LevelObjectPoolDataAsset)
    {
        UE_LOG(LogObjectPool, Error, TEXT("LevelObjectPoolDataAsset is null"));
        return false;
    }

    //Log info
    UE_LOG(LogObjectPool, Log, TEXT("Regist LevelObjectPoolDataAsset [%s] in Level [%s], levelptr: [%d]")
        , *LevelObjectPoolDataAsset->GetName(), *Level->GetName(), Level.Get());

    //caculate the total number of objects in the LevelObjectPoolDataAsset
    uint32 TotalNum = 0;
    for (auto& ClassData : LevelObjectPoolDataAsset->PoolClassArray)
    {
        //Check if the ActorClass is null in PoolClassDataAsset
        if (!ClassData->Class.Get())
        {
            UE_LOG(LogObjectPool, Error, TEXT("ActorClass in PoolClassDataAsset [%s] in LevelObjectPoolDataAsset [%s] is null")
                , *ClassData->GetName(), *LevelObjectPoolDataAsset->GetName());
            return false;
        }
        TotalNum += ClassData->PreallocateCount;
    }

    //add a new chunkpool for this LevelObjectPoolDataAsset
    ChunkPoolArray& ChunkPool = Map_ChunkPool.FindOrAdd(LevelObjectPoolDataAsset);
    TSharedPtr<FChunkPool> pool = MakeShareable(new FChunkPool(TotalNum));
    ChunkPool.Add(pool);


    uint32 PosInChunkPool = 0;
    //Traversal all the class in the LevelObjectPoolDataAsset
    for (auto& ClassData : LevelObjectPoolDataAsset->PoolClassArray)
    {
        //Check if the pool of this class has been created
        TObjectPtr<USingleClassObjectPool> classinpool = nullptr;
        if (!Map_ClassPool.Contains(ClassData->Class))
        {
            classinpool = NewObject<USingleClassObjectPool>(this);
            classinpool->Init(ClassData->Class,Level.Get());
            Map_ClassPool.Add(ClassData->Class, classinpool);
        }
        else
        {
            classinpool = Map_ClassPool[ClassData->Class];
        }

        //SingleClassPool register
        bool flag = classinpool->Register(ClassData, pool, PosInChunkPool, this);
        if (!flag)
        {
            UE_LOG(LogObjectPool, Error, TEXT("Regist PoolClassData [%s] LevelObjectPoolDataAsset [%s] in Level [%s] failed")
                , *ClassData->GetName(), *LevelObjectPoolDataAsset->GetName(), *Level->GetName());
            return false;
        }
        PosInChunkPool += ClassData->PreallocateCount;
    }

    UE_LOG(LogObjectPool, Warning, TEXT("Regist LevelObjectPoolDataAsset [%s] in Level [%s] success")
           , *LevelObjectPoolDataAsset->GetName(), *Level->GetName());
    return true;
}

void ULevelObjectPool::UnRegisterLevelObjectPool(ULevelObjectPoolDataAsset* LevelObjectPoolDataAsset)
{
    if (nullptr == LevelObjectPoolDataAsset)
    {
        UE_LOG(LogObjectPool, Error, TEXT("LevelObjectPoolDataAsset is null"));
        return;
    }

    //make sure the LevelObjectPoolDataAsset has been registered
    if (!Map_ChunkPool.Contains(LevelObjectPoolDataAsset))
    {
        UE_LOG(LogObjectPool, Error, TEXT("Try to unregister LevelObjectPoolDataAsset [%s] in Level [%s] failed, because the LevelObjectPoolDataAsset has not been registered")
                   , *LevelObjectPoolDataAsset->GetName(), *Level->GetName());
        return;
    }

    //Log info
    UE_LOG(LogObjectPool, Log, TEXT("UnRegist LevelObjectPoolDataAsset [%s] in Level [%s]")
            , *LevelObjectPoolDataAsset->GetName(), *Level->GetName());
    
    //clear all singleclasspool registed by LevelObjectPoolDataAsset in this level
    for (auto& ClassData : LevelObjectPoolDataAsset->PoolClassArray)
    {
        //Check if the ActorClass is null in PoolClassDataAsset
        if (!ClassData->Class.Get())
        {
            UE_LOG(LogObjectPool, Error, TEXT("ActorClass in PoolClassDataAsset [%s] in LevelObjectPoolDataAsset [%s] is null")
                , *ClassData->GetName(), *LevelObjectPoolDataAsset->GetName());
        }

        //Check if the pool of this class has been created
        if (!Map_ClassPool.Contains(ClassData->Class))
        {
            UE_LOG(LogObjectPool, Error, TEXT("Try to unregister PoolClassData [%s] LevelObjectPoolDataAsset [%s] in Level [%s] failed, because the pool of this class has not been created")
                , *ClassData->GetName(), *LevelObjectPoolDataAsset->GetName(), *Level->GetName());
            continue;
        }

        TObjectPtr<USingleClassObjectPool> classinpool = Map_ClassPool[ClassData->Class];

        //SingleClassPool unregister
        classinpool->UnRegister(ClassData);
        //Log info
        UE_LOG(LogObjectPool, Log, TEXT("UnRegist PoolClassData [%s] LevelObjectPoolDataAsset [%s] in Level [%s] success")
                , *ClassData->GetName(), *LevelObjectPoolDataAsset->GetName(), *Level->GetName());
        
        //check if this classinpool need to remove
        //classinpool->MarkAsGarbage();
    }

    //release the chunkpool for this LevelObjectPoolDataAsset
}

UUserWidget* ULevelObjectPool::SetLevelObjectPoolUIByClass(TSubclassOf<ULevelObjectPoolUI> uiClass)
{
    APlayerController* control = this->GetWorld()->GetGameInstance()->GetFirstLocalPlayerController();
    if (control)
    {
        LevelObjectPoolUI = CreateWidget<ULevelObjectPoolUI>(control, uiClass);
    }
    else
    {
        UE_LOG(LogObjectPool, Error, TEXT("SpawnTestActor::BeginPlay: control is null"));
    }

    if (!LevelObjectPoolUI)
    {
        UE_LOG(LogObjectPool, Error, TEXT("SpawnTestActor::BeginPlay: LevelObjectPoolUI is null"));
        return nullptr;
    }

    SetLevelObjectPoolUI(LevelObjectPoolUI);

    return LevelObjectPoolUI;
}

void ULevelObjectPool::SetLevelObjectPoolUI(ULevelObjectPoolUI* ui)
{
    if (!ui)
    {
        UE_LOG(LogObjectPool, Warning, TEXT("SetLevelObjectPoolUI: ui is null"));
        return;
    }
    LevelObjectPoolUI = ui;
    LevelObjectPoolUI->LevelObjectPoolData.LevelName = Level->GetName();
    LevelObjectPoolUI->LevelObjectPoolData.ObjectClassCount = Map_ClassPool.Num();

    UpdateData();
}

UObject* ULevelObjectPool::AllocateObject(UClass* Class)
{
    if (!Map_ClassPool.Contains(Class))
    {
        UE_LOG(LogObjectPool, Error, TEXT("Try to allocate object of class [%s] in Level [%s] failed, because the pool of this class has not been created")
                   , *Class->GetName(), *Level->GetName());
        return nullptr;
    }

    //a counter to call UpdateData every 100 times
    static int32 Counter = 0;
    if (Counter++ % 100 == 0)
    {
        UpdateData();
        Counter = 0;
    }
    return Map_ClassPool[Class]->AllocateObject();
}

void ULevelObjectPool::TickPool(float DeltaTime)
{
    for (auto& ClassPool : Map_ClassPool)
    {
        ClassPool.Value->Tick(DeltaTime);
    }
}

void ULevelObjectPool::UpdateData()
{
    if (!LevelObjectPoolUI)
    {
        return;
    }

    LevelObjectPoolUI->SingleClassPoolData.Empty();
    for (auto& ClassPool : Map_ClassPool)
    {
        FSingleClassPoolData data;
        data.ClassName = ClassPool.Key->GetName();
        data.TotalObjectCount = ClassPool.Value->getallcount();
        data.ReadyObjectCount = ClassPool.Value->getreadycount();
        data.InUseObjectCount = ClassPool.Value->getusedcount();
        data.InPoolObjectCount = ClassPool.Value->getfreecount();
        LevelObjectPoolUI->SingleClassPoolData.Add(data);
    }
}
