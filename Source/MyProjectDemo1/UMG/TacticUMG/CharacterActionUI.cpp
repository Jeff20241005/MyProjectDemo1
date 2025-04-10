// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterActionUI.h"
#include "SkillListUI.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "MyProjectDemo1/Characters/TacticBaseCharacter.h"
#include "MyProjectDemo1/Characters/TacticPlayerCharacter.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
#include "MyProjectDemo1/UMG/Base/ActionButtonUI.h"
#include "MyProjectDemo1/UMG/Base/ActionCheckBoxUI.h"


void UCharacterActionUI::ActionButton_SkillFunction()
{
	SkillListUI->SetVisibility(ESlateVisibility::Visible);
	if (ATacticPlayerCharacter* TacticPlayerCharacter = TacticSubsystem->TryGetActionPlayer())
	{
		SkillListUI->GenerateList(TacticPlayerCharacter);
	}
}

void UCharacterActionUI::TestSwitchCharac()
{
	TacticSubsystem->SortCharactersByActionValues();

	auto FirstCharacter = TacticSubsystem->GetAllCharactersInOrder()[0];
	if (FirstCharacter)
	{
		TacticSubsystem->OnSwitchToNextCharacterAction.Broadcast();
		//直接设置ActionValue，代表执行完毕Action。
		FirstCharacter->GetBaseCharacterAttributeSet()->SetActionValues(0);
	}
}

void UCharacterActionUI::PreMove(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility)
{
	BP_PreMove();
}

void UCharacterActionUI::CancelPreMove()
{
	BP_CancelMove();
}

void UCharacterActionUI::ActionButton_MoveOnClick()
{
	TacticSubsystem->OnCancelSkill.Broadcast();

	ATacticPlayerController* TacticPlayerController =
		GetWorld()->GetFirstPlayerController<ATacticPlayerController>();
	TacticSubsystem->OnPreMove.Broadcast(TacticPlayerController, nullptr);
}

void UCharacterActionUI::ActionCheckBoxUI_CheckBoxOnCheckStateChange(bool bIsChecked)
{
	TacticSubsystem->OnChangeAutomaticMoveBySkill.Broadcast(bIsChecked);
}

void UCharacterActionUI::NativeConstruct()
{
	Super::NativeConstruct();

	TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>();

	//todo better or not to put show move / BeginDrawVisualFeedBack in userwidgets
	ActionButton_Move->Button->OnClicked.AddDynamic(this, &ThisClass::ActionButton_MoveOnClick);
	// we may move the tactic "real" move function, to ability base

	//ActionButton_Move->Button->OnClicked.AddDynamic(this, &ThisClass::SetCurrent);
	//ActionButton_Attack->Button->OnClicked.AddDynamic(TacticSubsystem, &UTacticSubsystem::);
	ActionButton_SwitchCharacter->Button->OnClicked.AddDynamic(this, &ThisClass::TestSwitchCharac);
	ActionButton_Skill->Button->OnClicked.AddDynamic(this, &ThisClass::ActionButton_SkillFunction);

	TacticSubsystem->OnPreMove.AddUObject(this, &ThisClass::PreMove);
	TacticSubsystem->OnCancelMove.AddUObject(this, &ThisClass::CancelPreMove);

	ActionCheckBoxUI_ChangeAutomaticMoveBySkill->CheckBox->OnCheckStateChanged.AddDynamic(
		this, &ThisClass::ActionCheckBoxUI_CheckBoxOnCheckStateChange);
}
