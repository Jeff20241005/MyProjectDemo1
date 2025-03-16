// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterActionUI.h"

#include "Components/Button.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
#include "MyProjectDemo1/UMG/ActionButton.h"

void UCharacterActionUI::NativeConstruct()
{
	Super::NativeConstruct();
	TacticSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTacticSubsystem>();

	//todo better or not to put show move / BeginDrawVisualFeedBack in userwidgets
	ActionButton_Move->Button->OnClicked.AddDynamic(TacticSubsystem, &UTacticSubsystem::ShowMove);
	//ActionButton_Attack->Button->OnClicked.AddDynamic(TacticSubsystem, &UTacticSubsystem::);
	//ActionButton_Skill->Button->OnClicked.AddDynamic(TacticSubsystem, &UTacticSubsystem::);
}
