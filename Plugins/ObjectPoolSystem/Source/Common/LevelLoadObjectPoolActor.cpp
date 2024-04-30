// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelLoadObjectPoolActor.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "Macro/ObjectPoolSystemMacro.h"
#include "DataAsset/LevelObjectPoolDataAsset.h"
#include "ObjectPool/LevelObjectPoolUI.h"
#include "ObjectPool/LevelObjectPool.h"
#include "ObjectPoolSubsystem.h"

// Sets default values
ALevelLoadObjectPoolActor::ALevelLoadObjectPoolActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelLoadObjectPoolActor::BeginPlay()
{
    if (PoolDataAsset)
    {
        ULevel* level = GetLevel();
        check(level);
        bool flag = PoolDataAsset->RegisterToObjectPoolSubsystem(level);
        if (flag)
        {
            UE_LOG(LogObjectPool, Log, TEXT("Regist To Pool Success"));
            if (bShowPoolInfoUI)
            {
                LoadPoolInfoUI();
            }
        }
        else
        {
            UE_LOG(LogObjectPool, Error, TEXT("Regist To Pool Failed"));
        }
    }
    else
    {
        UE_LOG(LogObjectPool, Error, TEXT("PoolDataAsset is null"));
    }

    Super::BeginPlay();
}

void ALevelLoadObjectPoolActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (PoolDataAsset)
    {
        PoolDataAsset->UnRegisterFromObjectPoolSubsystem(GetLevel());
    }
    else
    {
        UE_LOG(LogObjectPool, Error, TEXT("PoolDataAsset is null"));
    }
}

// Called every frame
void ALevelLoadObjectPoolActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelLoadObjectPoolActor::LoadPoolInfoUI()
{
    APlayerController* controller = GetGameInstance()->GetFirstLocalPlayerController();
    if (controller && PoolInfoUIClass)
    {
        PoolInfoUI = CreateWidget<ULevelObjectPoolUI>(controller, PoolInfoUIClass);
        if (PoolInfoUI)
        {
            ULevelObjectPool* pool = GetWorld()->GetSubsystem<UObjectPoolSubsystem>()->GetLevelObjectPool(GetLevel());
            check(pool);
            pool->SetLevelObjectPoolUI(PoolInfoUI);
            UGameViewportSubsystem* viewport = GetGameInstance()->GetEngine()->GetEngineSubsystem<UGameViewportSubsystem>();
            check(viewport);
            viewport->AddWidget(PoolInfoUI, FGameViewportWidgetSlot());
        }
        else
        {
            UE_LOG(LogObjectPool, Error, TEXT("Create PoolInfoUI Failed"));
        }
    }
    else
    {
        UE_LOG(LogObjectPool, Error, TEXT("PoolInfoUIClass is null"));
    }
}

