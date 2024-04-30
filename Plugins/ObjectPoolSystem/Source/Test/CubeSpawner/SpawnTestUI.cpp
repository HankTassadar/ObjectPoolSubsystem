// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnTestUI.h"
#include "Private/Macro/ObjectPoolSystemMacro.h"

void USpawnTestUI::ResetAll()
{
    UIData.ResetAll();
    OnDataChange.Broadcast();
}

void USpawnTestUI::SwitchUseObjectPool()
{
    UIData.bUseObjectPool = !UIData.bUseObjectPool;
    OnDataChange.Broadcast();
}

void USpawnTestUI::SwitchStartSpawn()
{
    UIData.bStartSpawn = !UIData.bStartSpawn;
    OnDataChange.Broadcast();
}

void USpawnTestUI::SwitchBlueprintSpawn()
{
    UIData.bBlueprintSpawn = !UIData.bBlueprintSpawn;
    OnDataChange.Broadcast();
}

void USpawnTestUI::ChangeSpawnCountPerTick(const FText& test)
{
    if (!FCString::IsNumeric(*test.ToString()))
    {
        return;
    }
    UIData.SpawnCountPerTick = FCString::Atoi(*test.ToString());
    OnDataChange.Broadcast();
}

void USpawnTestUI::ChangeStayCount(const FText& test)
{
    if (!FCString::IsNumeric(*test.ToString()))
    {
        return;
    }
    UIData.StayCount = FCString::Atoi(*test.ToString());
    OnDataChange.Broadcast();
}

void USpawnTestUI::AddHandleOnUIDataChange(const FScriptDelegate& handle)
{
    OnDataChange.Add(handle);
}


void USpawnTestUI::ClearHandleOnUIDataChange()
{
    OnDataChange.Clear();
}

