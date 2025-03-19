// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterActionUI.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Framework/GameModes/MyGameMode.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
#include "MyProjectDemo1/UMG/ActionButtonUI.h"

/* todo put it in buttonbase
for (auto Spec : GetActivatableAbilities())
	{
		if (UBaseAbility* Ability = Cast<UBaseAbility>(Spec.Ability))
		{
			if (UButtonBase* SkillWidget = CreateWidget<UButtonBase>(GetWorld(), DefaultSkillClass))
			{
				//---也能输入技能图像
				SkillWidget->SetupButton(Ability->AbilityName.ToString());
				SkillWidget->SetupButtonOnClick(WidgetOwner, Ability);
				SkillListBox->AddChild(SkillWidget);
			}
		}
	}
 */
void UCharacterActionUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	TacticSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTacticSubsystem>();

	//todo better or not to put show move / BeginDrawVisualFeedBack in userwidgets
//todo 	ActionButton_Move->Button->OnClicked.AddDynamic(TacticSubsystem, &UTacticSubsystem::);
	// we may move the tactic "real" move function, to ability base
	
	//ActionButton_Move->Button->OnClicked.AddDynamic(this, &ThisClass::SetCurrent);
	
	//ActionButton_Attack->Button->OnClicked.AddDynamic(TacticSubsystem, &UTacticSubsystem::);
	//ActionButton_Skill->Button->OnClicked.AddDynamic(TacticSubsystem, &UTacticSubsystem::);

	AMyGameMode* MyGameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	ActionButton_SwitchGameModeTest->Button->OnClicked.AddDynamic(MyGameMode,&AMyGameMode::SwitchControlMode);
	
	ActionButton_SwitchCharacter->Button->OnClicked.AddDynamic(TacticSubsystem,&UTacticSubsystem::TestFunc_SwitchCharacter_RanOutOfAction);
}
