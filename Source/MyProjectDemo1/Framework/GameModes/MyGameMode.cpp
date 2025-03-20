// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"

#include "GameFramework/SpectatorPawn.h"
#include "MyProjectDemo1/Framework/Controllers/FreeRoamPlayerController.h"
#include "MyProjectDemo1/Framework/Controllers/MyPlayerController.h"
#include "MyProjectDemo1/Framework/Controllers/TacticPlayerController.h"

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();
	SetControlMode(EControlMode::TacticalMode);
}

void AMyGameMode::SwitchControlMode()
{
	if (IsFreeRoamMode())
	{
		SetControlMode(EControlMode::TacticalMode);
	}
	else
	{
		SetControlMode(EControlMode::FreeRoamMode);
	}
}

void AMyGameMode::SetControlMode(EControlMode NewMode)
{
	if (!HasAuthority()) return; // 确保服务端执行

	// 清理旧控制器（关键改进点）
	if (APlayerController* OldController = GetWorld()->GetFirstPlayerController())
	{
		OldController->UnPossess(); // 解除Pawn控制
		OldController->Destroy(); // 彻底销毁旧控制器
	}

	APlayerController* NewController = nullptr;
	switch (NewMode)
	{
	case EControlMode::FreeRoamMode:
		NewController = GetWorld()->SpawnActor<AFreeRoamPlayerController>();
		break;

	case EControlMode::TacticalMode:
		NewController = GetWorld()->SpawnActor<ATacticPlayerController>();
		break;

	case EControlMode::LevelMode:
		//NewController = GetWorld()->SpawnActor<ALevelModePlayerController>();
		break;
	}
	CurrentControlMode = NewMode;
}
