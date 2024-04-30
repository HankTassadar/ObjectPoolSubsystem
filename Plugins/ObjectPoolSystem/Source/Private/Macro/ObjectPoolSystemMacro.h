// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


DECLARE_LOG_CATEGORY_CLASS(LogObjectPool, Log, All)

#define USE_UE_LOG_WITH_FILE_LINE_FUNCTION  //If you want to use UE_LOG with file, line and function, uncomment this line

//disable warning C4005
#pragma warning(disable:4005)

#ifdef USE_UE_LOG_WITH_FILE_LINE_FUNCTION
//Redefine UE_LOG to add file, line and function
#define UE_LOG( LogCategroy, Verbosity, Format, ... ) \
    UE_CLOG(true, LogCategroy, Verbosity, TEXT("%s:%d [%s] ") Format, TEXT(__FILE__), __LINE__, TEXT("" __FUNCTION__ ""), ##__VA_ARGS__);
#endif // USE_UE_LOG_WITH_FILE_LINE_FUNCTION

#pragma warning(default:4005)