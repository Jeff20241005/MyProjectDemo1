// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterActionUI.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Framework/GameModes/MyGameMode.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
#include "MyProjectDemo1/UMG/ActionButtonUI.h"

void UCharacterActionUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	TacticSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTacticSubsystem>();

	//todo better or not to put show move / BeginDrawVisualFeedBack in userwidgets
	ActionButton_Move->Button->OnClicked.AddDynamic(TacticSubsystem, &UTacticSubsystem::ShowVisualFeedback_Move);
	// we may move the tactic "real" move function, to ability base
	//todo add a bcanmove subsystem, then click to let character move, remove the function inside of ai controller
	//ActionButton_Move->Button->OnClicked.AddDynamic(this, &ThisClass::SetCurrent);
	
	//ActionButton_Attack->Button->OnClicked.AddDynamic(TacticSubsystem, &UTacticSubsystem::);
	//ActionButton_Skill->Button->OnClicked.AddDynamic(TacticSubsystem, &UTacticSubsystem::);

	AMyGameMode* MyGameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	ActionButton_SwitchGameModeTest->Button->OnClicked.AddDynamic(MyGameMode,&AMyGameMode::SwitchControlMode);
}
