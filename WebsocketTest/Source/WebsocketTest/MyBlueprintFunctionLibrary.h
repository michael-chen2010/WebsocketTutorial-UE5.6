// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class WEBSOCKETTEST_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    /**
     * 获取当前 PIE (Play In Editor) 实例的 ID。
     * 如果不在 PIE 模式下运行，则返回 -1。
     * @return PIE 实例的整数 ID。
     */
    UFUNCTION(BlueprintPure, Category = "Editor|PIE")
    static int32 GetPIEInstanceID();
};
