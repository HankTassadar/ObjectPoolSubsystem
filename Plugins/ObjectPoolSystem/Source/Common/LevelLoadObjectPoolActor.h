// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelLoadObjectPoolActor.generated.h"

class ULevelObjectPoolDataAsset;
class ULevelObjectPoolUI;
class ULevelObjectPool;

UCLASS()
class OBJECTPOOLSYSTEM_API ALevelLoadObjectPoolActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelLoadObjectPoolActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    void LoadPoolInfoUI();
public:
    UPROPERTY(EditDefaultsOnly, Category = "Object Pool")
    TObjectPtr<ULevelObjectPoolDataAsset> PoolDataAsset;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object Pool")
    bool bShowPoolInfoUI = false;

    UPROPERTY(EditDefaultsOnly, Category = "Object Pool")
    TSubclassOf<ULevelObjectPoolUI> PoolInfoUIClass;

    UPROPERTY(BluePrintReadOnly, Category = "Object Pool")
    TObjectPtr<ULevelObjectPoolUI> PoolInfoUI;
};
