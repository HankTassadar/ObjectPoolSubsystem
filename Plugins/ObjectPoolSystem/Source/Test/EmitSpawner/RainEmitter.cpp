// Fill out your copyright notice in the Description page of Project Settings.


#include "RainEmitter.h"

FTransform URainEmitter::GenerateSpawnTransform()
{
    //Generate Location and Rotation
    //Location is in a plane
    FTransform transform;
    FVector2D location(FMath::FRand(), FMath::FRand());
    transform.SetLocation(FVector(location.X * SpawnRange.X, location.Y * SpawnRange.Y, 0));
    return transform;
}
