
# ObjectPoolSubsystem

An ObjectPool For Unreal Engine 5

## 1.How to use

Drag the **BP_LoadObjectPool** actor into the level, it will automatically register the object pool defined by **DA_LevelObjectPool_FistPersonMap**.Then you can use SpawnActor or GetUObject in ObjectPoolSubsystem.

## 2.Classes

### 2.1 DataAsset

#### 2.1.1 PoolClassDataAsset

This class is used to define a pool class, the class in this DataAsset must implement the interface **IPoolInterface**.

#### 2.1.2 LevelObjectPoolDataAsset

This is a array of PoolClassDataAsset, it is used to define a pool in a level, you can register multiple pools in a level by spawn and destroy **LevelLoadObjectPoolActor**.

### 2.2 Interface

#### 2.2.1 IPoolInterface

This is a base interface for the class that you want to use in the object pool, if you want any object to be pooled, you must implement this interface.

#### 2.2.2 IPoolActorInterface

This is a subclass of IPoolInterface, it is used to define the actor's behavior when it is spawned or despawned.

#### 2.2.3 Your Own PoolObjectInterface

You can define your own interface by inheriting from IPoolInterface, such as **IPoolPawnInterface**, **IPoolCharacterInterface**. But notice that you must really know the lifetime and behavior of the object you want to pool, you can see details in the **IPoolActorInterface**, see what is done in function ***ReleaseToPool, AttachToLevel, OnObjectNew***, and you must do the same thing in your own interface.

### 2.3 Actors

#### 2.3.1 LevelLoadObjectPoolActor

This actor is a normal actor spawned by ***UWorld::SpawnActor***, used to register and unregister the object pool defined by **LevelObjectPoolDataAsset** when this actor BeginPlay and EndPlay. You can control the lifetime of the object pool by spawn and destroy this actor.

#### 2.3.2 PoolActor

This Actor can be spawned by ***UObjectPoolSubsystem::SpawnActor***, do not call this actor's ***Destroy*** function, use ***ReleaseToPool*** instead, if you called ***Destroy***, will crash in ***IPoolActorInterface::AbortActorDestroy***.
