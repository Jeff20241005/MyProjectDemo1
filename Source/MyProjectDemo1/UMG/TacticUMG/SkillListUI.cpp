// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillListUI.h"

#include "SkillUI.h"
#include "Components/ScrollBox.h"
#include "MyProjectDemo1/Characters/TacticPlayerCharacter.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"


void USkillListUI::GenerateList(ATacticPlayerCharacter* PlayerCharacter)
{
	TArray<FGameplayAbilitySpecHandle> OutAbilityHandles;
	TArray<FGameplayAbilitySpec> AllAbilities = PlayerCharacter->GetMyAbilityComp()->GetActivatableAbilities();
	for (FGameplayAbilitySpec
	     AbilitySpec : AllAbilities)
	{
		if (UBaseAbility* Ability = Cast<UBaseAbility>(AbilitySpec.Ability))
		{
			USkillUI* SkillUI = CreateWidget<USkillUI>(this, SkillUIClass);
			SkillUI->BaseAbility = Ability;
			
			ScrollBox->AddChild(SkillUI); //this have to be executed after pass the ability param
		}
	}
}
