// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"

class ABasePlayerController;
class ATacticPlayerController;
class AFreeRoamPlayerController;

// 游戏模式枚举
UENUM(BlueprintType)
enum class EControlMode : uint8
{
	FreeRoamMode, // 自由移动模式()
	TacticalMode, // 战棋模式
	LevelMode // 关卡模式
};

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameMode();
	
	UFUNCTION()
	void SwitchControlMode();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Mode")
	EControlMode CurrentControlMode = EControlMode::FreeRoamMode;

	UFUNCTION(BlueprintCallable)
	bool IsFreeRoamMode() { return CurrentControlMode == EControlMode::FreeRoamMode; }

	UFUNCTION(BlueprintCallable)
	bool IsTacticMode() { return CurrentControlMode == EControlMode::TacticalMode; }

	// 切换控制模式
	UFUNCTION(BlueprintCallable, Category = "Control Mode")
	void SetControlMode(EControlMode NewMode);

protected:
	virtual void BeginPlay() override;

	// 各种控制器类
	UPROPERTY(EditDefaultsOnly, Category = "Controllers")
	TSubclassOf<ATacticPlayerController> TacticalControllerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Controllers")
	TSubclassOf<AFreeRoamPlayerController> FreeRoamControllerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Controllers")
	TSubclassOf<ABasePlayerController> LevelControllerClass;

private:
	// 保存当前控制器的状态
	void SaveControllerState(APlayerController* Controller);

	// 恢复控制器状态
	void RestoreControllerState(APlayerController* Controller);
};
