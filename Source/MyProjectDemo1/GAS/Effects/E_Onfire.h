// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "E_Onfire.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UE_Onfire : public UGameplayEffect
{
	GENERATED_BODY()

	public:
		UE_Onfire();

		// 自定义效果 ID（例如：增加生命的效果）
		UPROPERTY()
		FString EffectID;

		// 可以在这里添加其他的自定义逻辑
	virtual void OnGameplayEffectChanged() override;
	

};
