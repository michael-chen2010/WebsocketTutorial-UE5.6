// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"

#include "Engine/Engine.h" // �ٴΰ�����ȷ���ֲ�����

int32 UMyBlueprintFunctionLibrary::GetPIEInstanceID()
{
    // GEngine ��һ��ָ��ǰ����ʵ����ȫ��ָ�롣
    // GetPIEInstance() �����᷵�ص�ǰ PIE ʵ����������
    // ��������� PIE ģʽ�����У����磬�������е���Ϸ�������᷵�� -1��
    // GetWorld() ��һ�� UE �ĺ꣬���ڻ�ȡ��ǰ�����ָ�롣
    return GWorld->GetOutermost()->GetPIEInstanceID();
}