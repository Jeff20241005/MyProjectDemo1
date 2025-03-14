// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"

#include "GameFramework/SpectatorPawn.h"
#include "MyProjectDemo1/Framework/Controllers/MyPlayerController.h"

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();
	MyPlayerController = GetWorld()->GetFirstPlayerController<AMyPlayerController>();
}

void AMyGameMode::SetControlMode(EControlMode NewMode)
{
	CurrentControlMode = NewMode;

	// 不需要切换Controller，只需要改变输入处理方式
	switch (CurrentControlMode)
	{
	case EControlMode::FreeRoamMode:
		// 启用WASD输入
		break;

	case EControlMode::TacticalMode:
		MyPlayerController->PossesSpawnedSpectatorPawn();
	// 禁用WASD输入
		break;
	}
}
