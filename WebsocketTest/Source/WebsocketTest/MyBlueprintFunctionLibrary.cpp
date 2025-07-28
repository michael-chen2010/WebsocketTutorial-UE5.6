// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"

#include "Engine/Engine.h" // 再次包含以确保局部定义

int32 UMyBlueprintFunctionLibrary::GetPIEInstanceID()
{
    // GEngine 是一个指向当前引擎实例的全局指针。
    // GetPIEInstance() 函数会返回当前 PIE 实例的索引。
    // 如果不是在 PIE 模式下运行（例如，独立运行的游戏），它会返回 -1。
    // GetWorld() 是一个 UE 的宏，用于获取当前世界的指针。
    return GWorld->GetOutermost()->GetPIEInstanceID();
}