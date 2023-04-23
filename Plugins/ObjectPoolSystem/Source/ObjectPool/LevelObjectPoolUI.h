// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "LevelObjectPoolUI.generated.h"

USTRUCT(BlueprintType)
struct FLevelObjectPoolData 
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "ObjectPoolUI")
    FString LevelName;

    UPROPERTY(BlueprintReadOnly, Category = "ObjectPoolUI")
    int32 ObjectClassCount;
};

USTRUCT(BlueprintType)
struct FSingleClassPoolData 
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "ObjectPoolUI")
    FString ClassName;

    //with ready and not ready object
    UPROPERTY(BlueprintReadOnly, Category = "ObjectPoolUI")
    int32 TotalObjectCount;

    //InUseCount + InPoolCount
    UPROPERTY(BlueprintReadOnly, Category = "ObjectPoolUI")
    int32 ReadyObjectCount;

    //is using 
    UPROPERTY(BlueprintReadOnly, Category = "ObjectPoolUI")
    int32 InUseObjectCount;

    //is in pool can be used
    UPROPERTY(BlueprintReadOnly, Category = "ObjectPoolUI")
    int32 InPoolObjectCount;
};

/**
 * 
 */
UCLASS()
class OBJECTPOOLSYSTEM_API ULevelObjectPoolUI : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "ObjectPoolUI")
    FLevelObjectPoolData LevelObjectPoolData;

    UPROPERTY(BlueprintReadOnly, Category = "ObjectPoolUI")
    TArray<FSingleClassPoolData> SingleClassPoolData;
};
