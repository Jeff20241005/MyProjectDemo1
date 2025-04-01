// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameMode.h"
#include "MyProjectDemo1/Framework/Controllers/BasePlayerController.h"
#include "MyProjectDemo1/Framework/Controllers/TacticPlayerController.h"
#include "MyProjectDemo1/Framework/Controllers/FreeRoamPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"

AMyGameMode::AMyGameMode()
{
	// 设置默认控制器类
	TacticalControllerClass = ATacticPlayerController::StaticClass();
	FreeRoamControllerClass = AFreeRoamPlayerController::StaticClass();
	LevelControllerClass = ABasePlayerController::StaticClass();
	
	// 设置默认游戏模式
	CurrentControlMode = EControlMode::FreeRoamMode;
}

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	//SetControlMode(CurrentControlMode);
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
	if (CurrentControlMode == NewMode)
		return;
	
	// 获取当前控制器
	APlayerController* OldController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!OldController)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get current player controller"));
		return;
	}
	
	// 保存当前控制器状态
	SaveControllerState(OldController);
	
	// 选择新的控制器类
	TSubclassOf<APlayerController> NewControllerClass = nullptr;
	switch (NewMode)
	{
		case EControlMode::TacticalMode:
			NewControllerClass = TacticalControllerClass;
			break;
		case EControlMode::FreeRoamMode:
			NewControllerClass = FreeRoamControllerClass;
			break;
		case EControlMode::LevelMode:
			NewControllerClass = LevelControllerClass;
			break;
	}
	
	// 创建新控制器
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	APlayerController* NewController = GetWorld()->SpawnActor<APlayerController>(
		NewControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	
	if (!NewController)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn new controller"));
		return;
	}
	
	/*// 转移控制权 todo
	APawn* CurrentPawn = OldController->GetPawn();
	if (CurrentPawn)
	{
		OldController->UnPossess();
		NewController->Possess(CurrentPawn);
	}*/
	
	// 恢复控制器状态
	RestoreControllerState(NewController);
	
	// 替换玩家控制器
	if (UGameplayStatics::GetPlayerController(GetWorld(), 0) == OldController)
	{
		UGameplayStatics::RemovePlayer(OldController, true);
		GetWorld()->GetFirstLocalPlayerFromController()->SwitchController(NewController);
	}
	
	// 更新游戏状态
	CurrentControlMode = NewMode;
}

void AMyGameMode::SaveControllerState(APlayerController* Controller)
{
	if (!Controller)
		return;
		
	// 保存视角目标
	AActor* ViewTarget = Controller->GetViewTarget();
	
	// 保存到PlayerState
	if (APlayerState* PlayerState = Controller->PlayerState)
	{
		// 可以使用自定义的PlayerState来存储更多信息
		// 例如：MyPlayerState->SavedViewTarget = ViewTarget;
	}
	
	// 也可以使用GameState来存储全局状态
}

void AMyGameMode::RestoreControllerState(APlayerController* Controller)
{
	if (!Controller)
		return;
		
	// 从PlayerState恢复状态
	if (APlayerState* PlayerState = Controller->PlayerState)
	{
		// 恢复之前保存的状态
		// 例如：AActor* ViewTarget = MyPlayerState->SavedViewTarget;
	}
	
	/*// 设置相同的视角目标
	APlayerController* OldController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (OldController && OldController != Controller)
	{
		AActor* ViewTarget = OldController->GetViewTarget();
		if (ViewTarget)
		{
			Controller->SetViewTarget(ViewTarget);
		}
	}*/
}
