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
     * ��ȡ��ǰ PIE (Play In Editor) ʵ���� ID��
     * ������� PIE ģʽ�����У��򷵻� -1��
     * @return PIE ʵ�������� ID��
     */
    UFUNCTION(BlueprintPure, Category = "Editor|PIE")
    static int32 GetPIEInstanceID();
};
