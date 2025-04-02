// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticHUD.h"

#include "MyProjectDemo1/UMG/TacticUMG/TacticMainUI.h"
#include "MyProjectDemo1/UMG/TacticUMG/CharacterActionUI.h"
#include "MyProjectDemo1/UMG/Base/ActionCheckBoxUI.h"

void ATacticHUD::BeginPlay()
{
	Super::BeginPlay();
	MakeUserWidget(MainUI, MainUIClass);
}

void ATacticHUD::ToggleAutomaticMoveBySkill()
{
	if (UActionCheckBoxUI* AutoMoveCheckBox = MainUI->CharacterActionUI->ActionCheckBoxUI_ChangeAutomaticMoveBySkill)
	{
		// 切换复选框状态
		AutoMoveCheckBox->ToggleCheckBox();
	}
}
