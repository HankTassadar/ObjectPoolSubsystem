// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolInterface.h"
#include "Private/Macro/ObjectPoolSystemMacro.h"

// Add default functionality here for any IPoolInterface functions that are not pure virtual.

void IPoolInterface::ReleaseToPool()
{
    check(pool);
    check(node);

    CastChecked<UObject>(this);
 
    pool->FreeObject(node);
}

void IPoolInterface::OnNewObject()
{
}

void IPoolInterface::OnObjectDestroy()
{
}
