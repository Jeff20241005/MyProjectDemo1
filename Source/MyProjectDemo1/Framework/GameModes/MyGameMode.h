// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyGameMode.generated.h"
/**
 * 
 */

class AMyPlayerController;

UENUM(BlueprintType)
enum class EControlMode : uint8
{
	FreeRoamMode, // 自由移动模式
	TacticalMode // 战棋模式
};

UCLASS()
class MYPROJECTDEMO1_API AMyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Mode")
	EControlMode CurrentControlMode=EControlMode::FreeRoamMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	AMyPlayerController* MyPlayerController;
	void SwitchControlMode();

	// 切换控制模式
	UFUNCTION(BlueprintCallable, Category = "Control Mode")
	void SetControlMode(EControlMode NewMode);
protected:
	virtual void BeginPlay() override;
};
