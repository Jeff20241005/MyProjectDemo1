// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticHUD.h"
#include "MyProjectDemo1/UMG/TacticUMG/TacticMainUI.h"
#include "MyProjectDemo1/UMG/TacticUMG/CharacterActionUI.h"
#include "MyProjectDemo1/UMG/Base/ActionCheckBoxUI.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"

void ATacticHUD::BeginPlay()
{
	Super::BeginPlay();
	MakeUserWidget(MainUI, MainUIClass);
}

void ATacticHUD::ToggleAutomaticMoveBySkill(UTacticSubsystem* TacticSubsystem)
{
	if (MainUI && MainUI->CharacterActionUI && 
	    MainUI->CharacterActionUI->ActionCheckBoxUI_ChangeAutomaticMoveBySkill)
	{
		// 获取复选框引用以简化代码
		UActionCheckBoxUI* AutoMoveCheckBox = MainUI->CharacterActionUI->ActionCheckBoxUI_ChangeAutomaticMoveBySkill;
		
		// 切换复选框状态
		AutoMoveCheckBox->ToggleCheckBox();
		
		// 更新子系统中的自动移动标志
		if (TacticSubsystem)
		{
			TacticSubsystem->bEnableAutomaticMoveBySkill = AutoMoveCheckBox->IsChecked();
		}
	}
}
