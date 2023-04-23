// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "SpawnTestUI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUIDataChange);

USTRUCT(BlueprintType)
struct FSpawnTestUIData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnTestUI | Data")
    int32 SpawnCountPerTick = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnTestUI | Data")
    int32 StayCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnTestUI | Data")
    bool bUseObjectPool = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnTestUI | Data")
    bool bStartSpawn = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnTestUI | Data")
    bool bBlueprintSpawn = false;

    void ResetAll() {
        SpawnCountPerTick = 10;
        StayCount = 0;
        bUseObjectPool = false;
        bStartSpawn = false;
        bBlueprintSpawn = false;
    }
};
/**
 * 
 */
UCLASS()
class OBJECTPOOLSYSTEM_API USpawnTestUI : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "SpawnTestUI")
    void ResetAll();

    UFUNCTION(BlueprintCallable, Category = "SpawnTestUI")
    void SwitchUseObjectPool();

    UFUNCTION(BlueprintCallable, Category = "SpawnTestUI")
    void SwitchStartSpawn();

    UFUNCTION(BlueprintCallable, Category = "SpawnTestUI")
    void SwitchBlueprintSpawn();

    UFUNCTION(BlueprintCallable, Category = "SpawnTestUI")
    void ChangeSpawnCountPerTick(const FText& test);

    UFUNCTION(BlueprintCallable, Category = "SpawnTestUI")
    void ChangeStayCount(const FText& test);

    void AddHandleOnUIDataChange(const FScriptDelegate& handle);

    void ClearHandleOnUIDataChange();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnTestUI | Data")
    FSpawnTestUIData UIData;

private:
    UPROPERTY()
    FOnUIDataChange OnDataChange;
};
