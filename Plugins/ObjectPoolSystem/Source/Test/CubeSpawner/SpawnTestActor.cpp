// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnTestActor.h"
#include "../Private/Macro/ObjectPoolSystemMacro.h"
#include <UMG/Public/Blueprint/GameViewportSubsystem.h>
#include "../PoolInterface/PoolInterface.h"
#include "../ObjectPoolSubsystem.h"

// Sets default values
ASpawnTestActor::ASpawnTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASpawnTestActor::BeginPlay()
{
    if (!SpawnTestUIInstance)
    {
        APlayerController* control = this->GetGameInstance()->GetFirstLocalPlayerController();
        if (control)
        {
            SpawnTestUIInstance = CreateWidget<USpawnTestUI>(control, SpawnTestUIClass);
        }
        else
        {
            UE_LOG(LogObjectPool, Error, TEXT("SpawnTestActor::BeginPlay: control is null"));
        }
    }

	Super::BeginPlay();

    if (SpawnTestUIInstance)
    {
        UIData = &SpawnTestUIInstance->UIData;
        auto viewsubsystem = GetGameInstance()->GetSubsystem<UGameViewportSubsystem>();
        if (viewsubsystem)
        {
            viewsubsystem->AddWidget(SpawnTestUIInstance, FGameViewportWidgetSlot());
            UE_LOG(LogObjectPool, Log, TEXT("SpawnTestActor::BeginPlay: AddWidget"));
        }
        else
        {
            UE_LOG(LogObjectPool, Error, TEXT("SpawnTestActor::BeginPlay: viewsubsystem is null"));
        }
        FOnUIDataChange::FDelegate handle;
        handle.BindUFunction(this, TEXT("UpdateData"));
        SpawnTestUIInstance->AddHandleOnUIDataChange(handle);

    }
    else
    {
        UE_LOG(LogObjectPool, Error, TEXT("SpawnTestActor::BeginPlay: SpawnTestUI is null"));
    }
}

void ASpawnTestActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (SpawnTestUIInstance)
    {
        SpawnTestUIInstance->ClearHandleOnUIDataChange();
    }
    
    Super::EndPlay(EndPlayReason);
}

// Called every frame
void ASpawnTestActor::Tick(float DeltaTime)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(ASpawnTestActor::Tick);
	Super::Tick(DeltaTime);
    TickSpawn();
    StayCountAutoDestroy();
}

void ASpawnTestActor::BlueprintSpawnNormal_Implementation()
{
}

void ASpawnTestActor::BlueprintSpawnObjectPool_Implementation()
{
}

void ASpawnTestActor::TickSpawn()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(ASpawnTestActor::TickSpawn);
    if (!UIData)
    {
        UE_LOG(LogObjectPool, Error, TEXT("SpawnTestActor::TickSpawn: UIData is null"));
        return;
    }
    if (UIData->bStartSpawn)
    {
        if (UIData->bBlueprintSpawn)
        {
            if (UIData->bUseObjectPool)
            {
                for (int32 i = 0; i < UIData->SpawnCountPerTick; ++i)
                {
                    BlueprintSpawnObjectPool();
                }
            }
            else
            {
                for (int32 i = 0; i < UIData->SpawnCountPerTick; ++i)
                {
                    BlueprintSpawnNormal();
                }
            }
        }
        else
        {
            if (UIData->bUseObjectPool)
            {
                for (int32 i = 0; i < UIData->SpawnCountPerTick; ++i)
                {
                    SpawnObjectPool();
                }
            }
            else
            {
                for (int32 i = 0; i < UIData->SpawnCountPerTick; ++i)
                {
                    SpawnNormal();
                }
            }
        }
    }

}

void ASpawnTestActor::SpawnNormal()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(ASpawnTestActor::SpawnNormal);
    UWorld* world = GetWorld();
    FVector location = GetSpawnLocation();
    AActor* actor = world->SpawnActor<AActor>(NormalSpawnClass, location, FRotator());
    if (actor)
    {
        ActorList.AddTail(actor);
    }
    else
    {
        UE_LOG(LogObjectPool, Error, TEXT("SpawnTestActor::SpawnNormal: actor is null"));
    }
}

void ASpawnTestActor::SpawnObjectPool()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(ASpawnTestActor::SpawnObjectPool);
    UObjectPoolSubsystem* poolsubsystem = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();
    if (poolsubsystem)
    {
        FVector location = GetSpawnLocation();
        FTransform transform;
        transform.SetLocation(location);
        AActor* actor = nullptr;
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(ASpawnTestActor::SpawnObjectPool_SpawnActor);
            actor = poolsubsystem->SpawnActor(this, ObjectPoolSpawnClass, transform);
        }
        
        if (actor)
        {
            ActorList.AddTail(actor);
        }
        else
        {
            UE_LOG(LogObjectPool, Error, TEXT("SpawnTestActor::SpawnObjectPool: actor is null"));
        }
    }
    else
    {
        UE_LOG(LogObjectPool, Error, TEXT("SpawnTestActor::SpawnObjectPool: poolsubsystem is null"));
    }
}

void ASpawnTestActor::StayCountAutoDestroy()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(ASpawnTestActor::StayCountAutoDestroy);
    if (UIData && UIData->StayCount > 0)
    {
        for (int32 count = GetActorCount(); count > UIData->StayCount; count = GetActorCount())
        {
            DestroyOneActor();
        }
    }
}

uint32 ASpawnTestActor::GetActorCount()
{
    return ActorList.Num();
}

void ASpawnTestActor::DestroyOneActor()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(ASpawnTestActor::DestroyOneActor);
    if (ActorList.Num() > 0)
    {
        auto node = ActorList.GetHead();
        if (node)
        {
            AActor* actor = node->GetValue();
            if (actor)
            {
                if (actor->Implements<UPoolInterface>())
                {
                    IPoolInterface* poolactor = Cast<IPoolInterface>(actor);
                    if (poolactor)
                    {
                        TRACE_CPUPROFILER_EVENT_SCOPE(UPoolInterface::ReleaseToPool);
                        poolactor->ReleaseToPool();
                    }
                    else
                    {
                        UE_LOG(LogObjectPool, Error, TEXT("SpawnTestActor::DestroyOneActor: poolactor is null"));
                    }
                }
                else
                {
                    TRACE_CPUPROFILER_EVENT_SCOPE(AActor::Destroy);
                    actor->Destroy();
                }
            }
            else
            {
                UE_LOG(LogObjectPool, Error, TEXT("SpawnTestActor::DestroyOneActor: actor is null"));
            }
            ActorList.RemoveNode(node);
        }
    }
}

FVector ASpawnTestActor::GetSpawnLocation()
{
    FVector re = SpawnLocationInterval * SpawnPosition;
    USceneComponent* scene = GetRootComponent();
    if (scene)
    {
        re += scene->GetComponentLocation();
    }
    //move SpawnPosition to next point, order as x,y,z, limited by SpawnLocationCount
    SpawnPosition.X++;
    if (SpawnPosition.X >= SpawnLocationCount.X)
    {
        SpawnPosition.X = 0;
        SpawnPosition.Y++;
        if (SpawnPosition.Y >= SpawnLocationCount.Y)
        {
            SpawnPosition.Y = 0;
            SpawnPosition.Z++;
            if (SpawnPosition.Z >= SpawnLocationCount.Z)
            {
                SpawnPosition.Z = 0;
            }
        }
    }
    return re;
}

void ASpawnTestActor::AddToList(AActor* actor)
{
    if (actor)
    {
        ActorList.AddTail(actor);
    }
    else
    {
        UE_LOG(LogObjectPool, Error, TEXT("SpawnTestActor::AddToList: actor is null"));
    }
}

