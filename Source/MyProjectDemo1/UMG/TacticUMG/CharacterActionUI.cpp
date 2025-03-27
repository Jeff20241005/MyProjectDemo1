// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterActionUI.h"
#include "SkillListUI.h"
#include "Components/Button.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
#include "MyProjectDemo1/UMG/ActionButtonUI.h"


void UCharacterActionUI::ActionButton_SkillFunction()
{
	SkillListUI->SetVisibility(ESlateVisibility::Visible);
	if (APlayerCharacter* InCurrentControlPlayer = TacticSubsystem->CurrentControlPlayer)
	{
		SkillListUI->GenerateList(InCurrentControlPlayer);
	}
}

void UCharacterActionUI::TestSwitchCharac()
{
	TacticSubsystem->SortCharactersByActionValues();

	auto FirstCharacter = TacticSubsystem->GetAllCharactersInOrder()[0];
	if (TacticSubsystem->OnSwitchToNextCharacterAction.IsBound() && FirstCharacter)
	{
		TacticSubsystem->OnSwitchToNextCharacterAction.Broadcast();
		//直接设置ActionValue，代表执行完毕Action。
		FirstCharacter->GetBaseCharacterAttributeSet()->SetActionValues(0);
	}
}

void UCharacterActionUI::NativeConstruct()
{
	Super::NativeConstruct();

	TacticSubsystem = GetWorld()->GetSubsystem<UTacticSubsystem>();

	//todo better or not to put show move / BeginDrawVisualFeedBack in userwidgets
	//ActionButton_Move->Button->OnClicked.AddDynamic(TacticSubsystem, &UTacticSubsystem::);
	// we may move the tactic "real" move function, to ability base

	//ActionButton_Move->Button->OnClicked.AddDynamic(this, &ThisClass::SetCurrent);

	//ActionButton_Attack->Button->OnClicked.AddDynamic(TacticSubsystem, &UTacticSubsystem::);

	ActionButton_SwitchCharacter->Button->OnClicked.AddDynamic(this, &ThisClass::TestSwitchCharac);

	ActionButton_Skill->Button->OnClicked.AddDynamic(this, &ThisClass::ActionButton_SkillFunction);

	//AMyGameMode* MyGameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	//ActionButton_SwitchGameModeTest->Button->OnClicked.AddDynamic(MyGameMode, &AMyGameMode::SwitchControlMode);
}
