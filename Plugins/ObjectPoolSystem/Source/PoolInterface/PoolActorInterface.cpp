// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolActorInterface.h"
#include "GameFrameWork/Actor.h"
#include "Private/Macro/ObjectPoolSystemMacro.h"

void IPoolActorInterface::ReleaseToPool()
{
    //UE_LOG(LogObjectPool, Log, TEXT("Start ReleaseToPool"));

    //Get self as AActor
    AActor* self = GetActor();
    if (!self)
    {
        UE_LOG(LogObjectPool, Error, TEXT("Pool Actor Should release to pool but self is null, it is not an AActor's childclass"));
        //Todo: maybe should abort here, but not sure, so just return now
        return;
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(IPoolActorInterface::ReleaseToPool_DetachFromLevel);
        DetachFromLevel();
    }
        
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(IPoolActorInterface::ReleaseToPool_OnReleaseToPool);
        IPoolInterface::ReleaseToPool();
    }
}

void IPoolActorInterface::OnNewObject()
{
    //do something like init the object,but not regist tick and component
    //Get self as AActor
    AActor* self = GetActor();
    if (!self)
    {
        UE_LOG(LogObjectPool, Error, TEXT("Pool Actor Should attach to level but self is null, it is not an AActor's childclass"));
        return;
    }

    self->SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::Undefined;

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(IPoolActorInterface::AttachToLevel_PostSpawnInitialize);
        self->PostSpawnInitialize(FTransform(), nullptr, nullptr, false, false, true);
    }

    self->CheckDefaultSubobjects();

    //up there will add this actor in level, so here to detach it
    //this->ReleaseToPool();
}

void IPoolActorInterface::OnObjectDestroy()
{
    AActor* self = GetActor();
    if (!self)
    {
        UE_LOG(LogObjectPool, Error, TEXT("Pool Actor Should attach to level but self is null, it is not an AActor's childclass"));
        return;
    }

    self->MarkComponentsAsGarbage();
}

bool IPoolActorInterface::AttachToLevel(ULevel* levelSpawnIn, const FTransform& transform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride)
{
    if (!levelSpawnIn)
    {
        UE_LOG(LogObjectPool, Error, TEXT("Pool Actor Should attach to level but level is null"));
        return false;
    }

    //this is a new actor with managed by ObjectPoolSubsystem, so should not call actor's Destroy() funtion
    RegistOnActorDestroyCalledByWorld(levelSpawnIn->OwningWorld);

    //Get self as AActor
    AActor* self = GetActor();
    if (!self)
    {
        UE_LOG(LogObjectPool, Error, TEXT("Pool Actor Should attach to level but self is null, it is not an AActor's childclass"));
        return false;
    }

    self->SpawnCollisionHandlingMethod = CollisionHandlingOverride;

    if (bFirstTimeAttach)
    {
        bFirstTimeAttach = false;
        self->FinishSpawning(transform);
        return true;
    }

    self->GetRootComponent()->SetWorldTransform(transform);

    self->RegisterAllComponents();

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(IPoolActorInterface::DetachFromLevel_UnregisterAllActorTickFunctions);
        //register the actor's tick function
        const bool bRegisterTickFunctions = true;
        const bool bIncludeComponents = true;
        self->RegisterAllActorTickFunctions(bRegisterTickFunctions, bIncludeComponents);
    }

    self->PostActorConstruction();

    /*
    self->DispatchBeginPlay();*/
    

    //UE_LOG(LogObjectPool, Log, TEXT("AttachToLevel %s, transform: %s"), *self->GetName(), *transform.ToString
    return true;
}

void IPoolActorInterface::DetachFromLevel()
{
    //Get self as AActor
    AActor* self = GetActor();
    if (!self || !self->GetWorld())
    {
        UE_LOG(LogObjectPool, Error, TEXT("Pool Actor Should detach from level but self is null, it is not an AActor's childclass"));
        return;
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(IPoolActorInterface::ReleaseToPool_RouteEndPlay);
        self->RouteEndPlay(EEndPlayReason::RemovedFromWorld);
    }

    // Detach this actor's children
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(IPoolActorInterface::DetachFromLevel_DetachChildren);
        TArray<AActor*> AttachedActors;
        self->GetAttachedActors(AttachedActors);
        if (AttachedActors.Num() > 0)
        {
            TInlineComponentArray<USceneComponent*> SceneComponents;
            self->GetComponents(SceneComponents);

            for (TArray< AActor* >::TConstIterator AttachedActorIt(AttachedActors); AttachedActorIt; ++AttachedActorIt)
            {
                AActor* ChildActor = *AttachedActorIt;
                if (ChildActor != NULL)
                {
                    for (USceneComponent* SceneComponent : SceneComponents)
                    {
                        ChildActor->DetachAllSceneComponents(SceneComponent, FDetachmentTransformRules::KeepWorldTransform);
                    }
#if WITH_EDITOR
                    if (GIsEditor)
                    {
                        GEngine->BroadcastLevelActorDetached(ChildActor, self);
                    }
#endif
                }
            }
        }
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(IPoolActorInterface::DetachFromLevel_DetachFromActor);
        // Detach from anything we were attached to
        USceneComponent* RootComp = self->GetRootComponent();
        if (RootComp != nullptr && RootComp->GetAttachParent() != nullptr)
        {
            AActor* OldParentActor = RootComp->GetAttachParent()->GetOwner();
            if (OldParentActor)
            {
                // Attachment is persisted on the child so modify both actors for Undo/Redo but do not mark the Parent package dirty
                OldParentActor->Modify(/*bAlwaysMarkDirty=*/false);
            }

            self->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

#if WITH_EDITOR
            if (GIsEditor)
            {
                GEngine->BroadcastLevelActorDetached(self, OldParentActor);
            }
#endif
        }
    }

    // If this actor has an owner, notify it that it has lost a child.
    if (self->GetOwner())
    {
        self->SetOwner(NULL);
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(IPoolActorInterface::DetachFromLevel_UnregisterAllComponents);
        // Clean up the actor's components.
        self->UnregisterAllComponents();
        self->UninitializeComponents();
    }
    
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(IPoolActorInterface::DetachFromLevel_UnregisterAllActorTickFunctions);
        // Unregister the actor's tick function
        const bool bRegisterTickFunctions = false;
        const bool bIncludeComponents = false;
        self->RegisterAllActorTickFunctions(bRegisterTickFunctions, bIncludeComponents);
    }
    
}

FDelegateHandle IPoolActorInterface::AbortActorDestroyHandle;

void IPoolActorInterface::RegistOnActorDestroyCalledByWorld(UWorld* world)
{
    check(world);
    static bool bIsRegist = false;
    if (bIsRegist)
    {
        //Just regist once
        return;
    }
    AbortActorDestroyHandle = world->AddOnActorDestroyedHandler(
        FOnActorDestroyed::FDelegate::CreateRaw(this, &IPoolActorInterface::AbortActorDestroy)
    );
    bIsRegist = true;
}

//No need to move actually
void IPoolActorInterface::RemoveOnActorDestroyCalledByWorld(UWorld* world)
{
    check(world);
    world->RemoveOnActorDestroyededHandler(AbortActorDestroyHandle);
}

void IPoolActorInterface::AbortActorDestroy(AActor* self)
{
    if (!self->Implements<UPoolActorInterface>())
    {
        return;
    }
    UE_LOG(LogObjectPool, Fatal, TEXT("Pool Actor Should release to pool but destroy by world, abort!!!"));
}

AActor* IPoolActorInterface::GetActor()
{
    return CastChecked<AActor>(this);;
}

