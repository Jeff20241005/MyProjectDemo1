// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameMode.h"
#include "MyProjectDemo1/Framework/GameStates/TacticGameState.h"
#include "TacticGameMode.generated.h"

class ABaseCharacter;
class ATacticPlayerController;

/**
 * 
 */


UENUM()
enum ETacticState
{
	ETS_DefaultBattleState,
	
	ETS_BeginBattle,				//---进入战斗场景，设定胜利或者失败条件，读取角色和怪物信息等
	
	ETS_InBattle,					//---持续直至战斗结束
	
	ETS_CharacterAction,			//---角色行动
	ETS_EndCharacterAction,			//---结算BUFF时间
	
	ETS_EndBattle,					//---触发胜利或者失败条件，战斗结束
	ETS_Settlement,					//---结算画面
	
	ETS_ContinueBattle,				//---同上

};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterStateChange, ABaseCharacter*)

UCLASS()
//GameMode  游戏规则的定义和逻辑执行
class MYPROJECTDEMO1_API ATacticGameMode : public AMyGameMode
{
	GENERATED_BODY()

	
protected:
	//float DefaultTurnTimer = 1.0f;
	
	void BeginPlay() override;
	
public:
	FOnCharacterStateChange OnCharacterDeath;
	
	void InitPlayerCharacters();
	void InitAICharacters();
	void SetTacticStateEnum(ETacticState TacticState);

	void CharacterActionQueue();
	ETacticState CurrentBattleState;

private:
	UPROPERTY()
	ATacticGameState* TacticGameState;
	UPROPERTY()
	ATacticPlayerController* TacticPlayerController;		//---获取玩家控制器
};
