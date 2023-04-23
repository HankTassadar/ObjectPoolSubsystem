// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FObjectChunk
{
    FObjectChunk() {
        ReSet();
    }
    void ReSet()
    {
        Object = nullptr;
        bIsAllocated = false;
    }
    UObject* Object;
    bool bIsAllocated;
};

struct FChunkPool
{
    FChunkPool() = delete;
    FChunkPool(uint32 size) {
        ChunkArray.SetNum(size);
    }
    TArray<FObjectChunk> ChunkArray;
};