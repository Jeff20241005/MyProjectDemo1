// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterActionUI.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectDemo1/Characters/BaseCharacter.h"
#include "MyProjectDemo1/Framework/GameModes/MyGameMode.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
#include "MyProjectDemo1/UMG/ActionButtonUI.h"

void UCharacterActionUI::ActionButton_SkillFunction()
{
	if (ABaseCharacter* InCurrentControlPlayer = TacticSubsystem->CurrentActionCharacter)
	{
		//也许不断重新创建比较好？删除又创建，就不用手动清空技能列表了。
		/*todo 把InCurrentControlPlayer放入技能UI
		TArray<FGameplayAbilitySpecHandle> OutAbilityHandle;
		InCurrentControlPlayer->GetMyAbilityComp()->GetAllAbilities(OutAbilityHandle);

		for (FGameplayAbilitySpecHandle AbilityHandle : OutAbilityHandle)
		{
			//AbilityHandle.
		}*/
	}
}

void UCharacterActionUI::TestSwitchCharac()
{

	TacticSubsystem->SortCharactersByActionValues();
	
	auto FirstCharacter = TacticSubsystem->GetAllCharactersInOrder()[0];
	
	if (TacticSubsystem->OnSwitchCharacterAction.IsBound() && FirstCharacter)
	{
		TacticSubsystem->OnSwitchCharacterAction.Broadcast(FirstCharacter);
		//直接设置ActionValue，代表执行完毕Action。
		FirstCharacter->GetBaseCharacterAttributeSet()->SetActionValues(0);
	}
}

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

	ActionButton_SwitchCharacter->Button->OnClicked.AddDynamic(this, &ThisClass::TestSwitchCharac);
	//todo test can't bind subsystem , can not find subsystem on construction
	//todo2 test can't bind button.

	//ActionButton_Skill->Button->OnClicked.AddDynamic(this, &ThisClass::ActionButton_SkillFunction);

	//AMyGameMode* MyGameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	//ActionButton_SwitchGameModeTest->Button->OnClicked.AddDynamic(MyGameMode, &AMyGameMode::SwitchControlMode);
}
