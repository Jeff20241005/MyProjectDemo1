// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
* PlayerState 
*存储特定玩家的状态信息
 */
UCLASS()
class MYPROJECTDEMO1_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	// 战斗统计
	int TotalDamageDealt;
	int TotalHealingDone;
    
	// 玩家资源
	int Gold;
	int Experience;
};
