# Actor 的生成与销毁 ，以及 ActorPool 的必要性

 说明：以下示例代码来自于 Unreal Engine 5.1.1，ObjectPool代码以及测试蓝图来自 ***[对象池子系统插件](https://github.com/HankTassadar/ObjectPoolSubsystem "对象池子系统插件")*** ，以下关于性能的讨论只针对GameThread。

## 0. 概述

试想这样一个场景，我们有一个游戏，需要在游戏中生成大量的 Actor，比如子弹，怪物，道具等等，这些 Actor 都是需要在游戏中动态生成的，而且这些 Actor 的数量是非常大的，比如同一时刻存在的子弹的数量可能会达到几千个，每Tick都会生成数百个的生成与销毁。这时进行性能优化的时候，我们进需要从以下几个方面考虑：Actor的生成，Actor的Tick，Actor的销毁。

本文主要分析一些 Actor 在生成与销毁时的性能消耗，进而讨论一些关于 ActorPool 必要性的问题。

## 1. Actor 的生成与销毁

### 1.1 Actor 的生命周期

在 UE 中，Actor 的生命周期主要分为以下几个阶段：

    (1) New 出一个 Actor
    (2) Actor 初始化
    (3) BeginPlay
    (4) Tick，响应输入与事件
    (5) EndPlay
    (6) 销毁 Actor
    (7) Actor 被 GC 回收
上述过程中， Actor 的 New ，初始化与 BeginPlay 通常是一起完成的，当延迟生成时会分开完成，EndPlay 与销毁 Actor 通常是一起完成的。

### 1.2 Actor 的生成

我们可以通过调用 UWorld::SpawnActor 来生成一个 Actor，这个函数的声明如下：

```cpp
AActor* SpawnActor( UClass* Class, FTransform const* Transform, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());
```

想要生成一个 Actor ，我们只需要知道它的类型，以及生成时的坐标变换信息即可。

在这个函数当中，我们省略一些非重点代码，只关注Actor的流程：

```cpp
AActor* UWorld::SpawnActor( UClass* Class, FTransform const* UserTransformPtr, const FActorSpawnParameters& SpawnParameters)
{
...
//一串if判断，判断是否可以生成Actor
...
//编辑器相关，Actor名称等
...
//碰撞相关，是否允许发生碰撞生成
...
    // actually make the actor object
    AActor* const Actor = NewObject<AActor>(LevelToSpawnIn, Class, NewActorName, ActorFlags, Template, false/*bCopyTransientsFromClassDefaults*/, nullptr/*InInstanceGraph*/, ExternalPackage);
...
    if ( GUndo )
    {
        ModifyLevel( LevelToSpawnIn );
    }
    LevelToSpawnIn->Actors.Add( Actor );
    LevelToSpawnIn->ActorsForGC.Add(Actor);
...
    // Broadcast delegate before the actor and its contained components are initialized
    OnActorPreSpawnInitialization.Broadcast(Actor);

    Actor->PostSpawnInitialize(UserTransform, SpawnParameters.Owner, SpawnParameters.Instigator, SpawnParameters.IsRemoteOwned(), SpawnParameters.bNoFail, SpawnParameters.bDeferConstruction);
...
    Actor->CheckDefaultSubobjects();

    // Broadcast notification of spawn
    OnActorSpawned.Broadcast(Actor);
...
}
```

生成一个Actor的流程主要是 NewObject，然后添加到 Level 的 Actor 数组当中，然后调用 Actor 的 PostSpawnInitialize 函数，最后调用 Actor 的 CheckDefaultSubobjects 函数。

注意 PostSpawnInitialize 函数的调用时的最后一个参数 SpawnParameters.bDeferConstruction，这个参数的作用是是否延迟 Actor 生成到 Level 中，如果为 true ，则 Actor 不会被生成到 Level 中直到主动调用了 Actor 的 FinishSpawning 函数。

于是我们可以通过这里入手预先构造出 Actor，然后在需要的时候再调用 FinishSpawning 函数，这样就可以避免在游戏中频繁的生成 Actor 了。

### 1.3 Actor 生成的性能消耗

接下来我们使用  ***[对象池子系统插件](https://github.com/HankTassadar/ObjectPoolSubsystem "对象池子系统插件")*** 中的 BP_PerformanceTest 来测试一下生成过程中各个函数的性能消耗，测试中生成的是一个只有一个 StaticMeshComponent 的 Actor。简化 SpawnActor 的流程到 ActorSpawnPerformanceTest::SpawnActors 中，然后每 Tick 生成固定数量的 Actor ，然后销毁掉。使用 TRACE_CPUPROFILER_EVENT_SCOPE 宏来测量各个过程的消耗。

使用 Unreal Insights 查看性能消耗，截取其中一次 Tick 如下：
![image](./Image/Tick.PNG)

可以看到，多次 SpawnActor 然后 Destroy 时， 生成时的性能消耗占比较大（不一定，会有例外，之后会讨论销毁性能）。

然后我们再来看一下具体的单次 SpawnActor 消耗：
<details>
<summary>AActorSpawnPerformanceTest::SpawnActors</summary>

```cpp

AActor* AActorSpawnPerformanceTest::SpawnActors()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::SpawnActors);
    FName name = FName(*FString::Printf(TEXT("AActorSpawnPerformanceTest_Actors_%d"), SpawnNameUnique++));
    ULevel* level = GetLevel();
    AActor* actor = nullptr;

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::SpawnActors_NewObject);
        actor = NewObject<AActor>(level, ActorToSpawn, name, EObjectFlags::RF_Transactional);
    }

    if (!actor)
    {
        return nullptr;
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::SpawnActors_AddToLevel);
        level->Actors.Add(actor);
        level->ActorsForGC.Add(actor);
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::SpawnActors_PostSpawnInitialize);
        actor->PostSpawnInitialize(FTransform(), nullptr, nullptr, false, false, true);
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::SpawnActors_CheckDefaultSubobjects);
        actor->CheckDefaultSubobjects();
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(AActorSpawnPerformanceTest::SpawnActors_FinishSpawning);
        actor->FinishSpawning(FTransform());
    }

    return actor;
}
```

</details>

![image](./Image/SpawnActors.PNG)

可以看到大部分的消耗是在最后的 FinishSpawning 上。我们详细看一下这个函数。

```cpp
void AActor::FinishSpawning(const FTransform& UserTransform, bool bIsDefaultTransform, const FComponentInstanceDataCache* InstanceDataCache)
{
...
//一些非重点代码
...
    {
        FEditorScriptExecutionGuard ScriptGuard;
        ExecuteConstruction(FinalRootComponentTransform, nullptr, InstanceDataCache,bIsDefaultTransform);
    }

    {
        SCOPE_CYCLE_COUNTER(STAT_PostActorConstruction);
        PostActorConstruction();
    }
}
```
上述，如果生成的 Actor 比较复杂，那么性能消耗会更大。

## 2. Actor 的 Tick
