// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolInterface.h"
#include "PoolActorInterface.generated.h"

class ULevel;
class UWorld;
// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, NotBlueprintable)
class UPoolActorInterface : public UPoolInterface
{
	GENERATED_BODY()
};

/**
 * An Interface for turning Actorclass to an PoolActorclass so can used in ObjectPoolSubsystem
 */
class OBJECTPOOLSYSTEM_API IPoolActorInterface : public IPoolInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    
    /** release the actor */
    UFUNCTION(BlueprintCallable, Category = "ObjectPool", meta = (Keywords = "Release", DisplayName = "Release To Pool", ScriptName = "ReleaseToPool"))
    virtual void K2_ReleaseToPool(){ ReleaseToPool(); };
    
    /*
    * called to release this object back to ObjectPool
    */
    virtual void ReleaseToPool() override;

private:
    //when new actor, do some init work
    virtual void OnNewObject() override;

private:
    //the follows functions are only used by ObjectPoolSubsystem, so make it private
    friend class UObjectPoolSubsystem;

    /************************ interface for actor start ****************************************************/

    /*
    * Attach/Place this actor to level, called by ObjectPoolSubsystem
    * will call the BeginPlay for this object and it's components
    */
    virtual bool AttachToLevel(ULevel* levelSpawnIn, const FTransform& transform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined);

    /*
    * Detach this actor from level
    */
    virtual void DetachFromLevel();

    /************************ interface for actor end ******************************************************/

private:
    bool bFirstTimeAttach = true;

private:
    /************************ function to make sure not use Actor::Destory() start *************************/
    
    //when actor is destroyed, it will call Destroy() function, but we don't want this happen, so we need to abort it
    //also you can override the Destroy() function in your actor class to call ReleaseToPool()

    /*
    * should not call actor's Destroy() funtion, so regist a delegate to OnActorDestroy to abort
    * called when attach to the world
    */
    void RegistOnActorDestroyCalledByWorld(UWorld* world);

    /*
    * Remove the delegate from OnActorDestroy
    * called when detach from the world
    */
    void RemoveOnActorDestroyCalledByWorld(UWorld* world);

    /*
    * the real function to abort actor's Destroy() funtion
    */
    void AbortActorDestroy(AActor* self);

    static FDelegateHandle AbortActorDestroyHandle;
    /************************ function to make sure not use Actor::Destory() over **************************/


    //check if this is an Actor Object,if not return nullptr
    inline AActor* GetActor();

};
