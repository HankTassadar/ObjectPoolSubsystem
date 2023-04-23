// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../PoolInterface/PoolActorInterface.h"

#include "PoolActor.generated.h"

/*
* An Actor that can be pooled, managed by a ObjectPoolSubsystem, should use ReleaseToPool() instead of Destroy()
*/
UCLASS(BlueprintType, Blueprintable)
class OBJECTPOOLSYSTEM_API APoolActor : public AActor, public IPoolActorInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APoolActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
