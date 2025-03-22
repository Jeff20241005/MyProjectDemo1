// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Framework/GameModes/MyGameMode.h"

void UMyGameInstance::Init()
{
	Super::Init();
	
	// 初始化代码
}

void UMyGameInstance::SwitchControlMode()
{
	// 通过GameMode切换游戏模式
	AMyGameMode* GameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->SwitchControlMode();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get MyGameMode"));
	}
}

EControlMode UMyGameInstance::GetCurrentControlMode() const
{
	// 获取当前游戏模式
	AMyGameMode* GameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		return GameMode->CurrentControlMode;
	}
	
	// 默认返回自由漫游模式
	return EControlMode::FreeRoamMode;
}
