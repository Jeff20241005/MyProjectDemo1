// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyProjectDemo1/Framework/GameModes/MyGameMode.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 提供一个全局访问点来切换游戏模式
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void SwitchControlMode();

	// 获取当前游戏模式
	UFUNCTION(BlueprintPure, Category = "Game Flow")
	EControlMode GetCurrentControlMode() const;

protected:
	virtual void Init() override;
};
