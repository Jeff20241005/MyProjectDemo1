// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Characters/TacticPlayerCharacter.h"
#include "MyProjectDemo1/Framework/Controllers/TacticPlayerController.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"


void ATacticGameMode::BeginPlay()
{
	Super::BeginPlay();
	TacticPlayerController = Cast<ATacticPlayerController>
		(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	TacticGameState = Cast<ATacticGameState>(GetWorld()->GetGameState());
	
	//SetTacticStateEnum(BeginBattle);
}


void ATacticGameMode::CharacterActionQueue()
{
	/*
	if (CharacterArrayPairs[0].Value >= 0)
	{
		//BattleState = BattleStateEnum::BeginCharacterAction;
		//---行动前的函数---

		//BattleState = BattleStateEnum::CharacterAction;
		//---触发角色行动函数---
		PlayerControllerRef.Execute_IF_CharacterStartAction(PlayerControllerRef, CharacterArrayPairs[0].Key);

		//BattleState = BattleStateEnum::EndCharacterAction;
		//---行动后的函数---
		CharacterArrayPairs[0].Value -= 1.0f;
	}
	else
	{
		//---回合数加一
		for (auto QueueList : CharacterArrayPairs)
		{
			QueueList.Value += 1; //---加上角色速度
		}
	}

	if (BattleState == BattleStateEnum::InBattle) BattleManager();
	*/
}


void ATacticGameMode::InitAICharacters()
{
	//---加入战斗场景里AI的角色
}

void ATacticGameMode::SetTacticStateEnum(ETacticState TacticState)
{
	switch (TacticState)
	{
	case ETS_DefaultBattleState:
		break;
	case ETS_BeginBattle:
		InitAICharacters();
		InitPlayerCharacters();
		break;
	case ETS_InBattle:
		CharacterActionQueue();
		break;
	case ETS_CharacterAction:
		break;
	case ETS_EndCharacterAction:
		break;
	case ETS_EndBattle:
		break;
	case ETS_Settlement:
		break;
	case ETS_ContinueBattle:
		break;
	default: ;
	}
}



void ATacticGameMode::InitPlayerCharacters()
{
}
