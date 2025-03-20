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
	//SetTacticStateEnum(BeginBattle);
}



void ATacticGameMode::SetTacticStateEnum(ETacticState TacticState)
{
	switch (TacticState)
	{
	case ETS_DefaultBattleState:
		break;
	case ETS_BeginBattle:

		InitPlayerCharacters();
		break;
	case ETS_InBattle:
	
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
