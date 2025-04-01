// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCheckBoxUI.h"
#include "Components/CheckBox.h"

void UActionCheckBoxUI::ToggleCheckBox()
{
	if (CheckBox)
	{
		// 获取当前状态并切换
		bool bCurrentState = CheckBox->IsChecked();
		CheckBox->SetCheckedState(bCurrentState ? ECheckBoxState::Unchecked : ECheckBoxState::Checked);
	}
}

void UActionCheckBoxUI::SetIsChecked(bool bIsChecked)
{
	if (CheckBox)
	{
		CheckBox->SetCheckedState(bIsChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}
}

bool UActionCheckBoxUI::IsChecked() const
{
	if (CheckBox)
	{
		return CheckBox->IsChecked();
	}
	return false;
}
