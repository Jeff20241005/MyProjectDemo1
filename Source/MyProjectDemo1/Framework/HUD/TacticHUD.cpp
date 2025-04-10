// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticHUD.h"

#include "GameplayEffect.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MyProjectDemo1/Characters/TacticBaseCharacter.h"
#include "MyProjectDemo1/Components/MyAbilityComp.h"
#include "MyProjectDemo1/GAS/Abilities/BaseAbility.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
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
		AutoMoveCheckBox->ToggleCheckBox();
	}
}


bool ATacticHUD::CheckHasEnoughResources(ATacticBaseCharacter* SourceCharacter, UBaseAbility* BaseAbility)
{
	if (!SourceCharacter || !BaseAbility) return false;

	float ActionCost = 0.0f;
	float ManaCost = 0.0f;

	// 检查行动点是否足够
	bool bHasEnoughAction = CheckResourceForAttribute(
		SourceCharacter,
		BaseAbility,
		UBaseCharacterAttributeSet::GetActionValuesAttribute(),
		&UBaseCharacterAttributeSet::GetActionValues,
		ActionCost);

	// 检查魔法值是否足够
	bool bHasEnoughMana = CheckResourceForAttribute(
		SourceCharacter,
		BaseAbility,
		UBaseCharacterAttributeSet::GetManaAttribute(),
		&UBaseCharacterAttributeSet::GetMana,
		ManaCost);

	// 如果资源不足，显示提示信息
	if (!bHasEnoughAction || !bHasEnoughMana)
	{
		FString Message;
		if (!bHasEnoughAction)
		{
			Message = FString::Printf(TEXT("行动点不足! 需要: %.0f, 当前: %.0f"),
			                          ActionCost, SourceCharacter->GetBaseCharacterAttributeSet()->GetActionValues());

			// 这里可以添加UI变化代码，比如闪烁行动点显示等
		}

		if (!bHasEnoughMana)
		{
			if (!Message.IsEmpty()) Message.Append(TEXT("\n"));
			Message.Append(FString::Printf(TEXT("魔法值不足! 需要: %.0f, 当前: %.0f"),
			                               ManaCost, SourceCharacter->GetBaseCharacterAttributeSet()->GetMana()));

			// 这里可以添加UI变化代码，比如闪烁魔法值显示等
		}

		if (GEngine && !Message.IsEmpty())
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, Message, true, FVector2D(1.5, 1.5));
	}

	// 只有当两种资源都足够时才返回true
	return bHasEnoughAction && bHasEnoughMana;
}

template <class T>
bool ATacticHUD::CheckResourceForAttribute(ATacticBaseCharacter* Character, UBaseAbility* Ability,
                                           const FGameplayAttribute& Attribute, T GetterFunc, float& OutCost)
{
	if (!Character || !Ability) return false;

	UGameplayEffect* CostEffect = Ability->GetCostGameplayEffect();
	if (!CostEffect) return true; // 没有消耗效果，视为资源足够
	// 从GE中获取消耗值
	if (!GetModifierMagnitudeForAttribute(CostEffect, Attribute, OutCost))
	{
		// 如果不能从GE直接获取，则尝试使用计算类
		TSubclassOf<UGameplayModMagnitudeCalculation> CalculationClass = nullptr;

		// 遍历ModifierInfos找到对应属性的计算类
		for (const FGameplayModifierInfo& ModInfo : CostEffect->Modifiers)
		{
			if (ModInfo.Attribute == Attribute)
			{
				CalculationClass = ModInfo.ModifierMagnitude.GetCustomMagnitudeCalculationClass();
				break;
			}
		}

		if (CalculationClass)
		{
			if (!PredictResourceCost(Character, Ability, CalculationClass, OutCost))
			{
				// 如果预测失败，使用默认值
				OutCost = 0;
			}
		}
		else
		{
			// 找不到计算类，使用默认值
			OutCost = 0;
		}
	}

	OutCost = FMath::Abs(OutCost); // 确保消耗为正值

	const UBaseCharacterAttributeSet* AttributeSet = Character->GetBaseCharacterAttributeSet();
	if (!AttributeSet) return false;

	float CurrentValue = (AttributeSet->*GetterFunc)();
	{
		FString TempStr = FString::Printf(
			TEXT("Current %s has %f	, The Skill Need %f"), *Attribute.AttributeName, CurrentValue, OutCost);
		if (GEngine)GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Turquoise, TempStr, true, FVector2D(2, 2));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}
	return CurrentValue >= OutCost;
}

bool ATacticHUD::GetModifierMagnitudeForAttribute(const UGameplayEffect* GameplayEffect,
                                                  const FGameplayAttribute& Attribute, float& OutMagnitude)
{
	if (!GameplayEffect) return false;

	for (const FGameplayModifierInfo& ModInfo : GameplayEffect->Modifiers)
	{
		if (ModInfo.Attribute == Attribute)
		{
			// 尝试获取静态幅度值
			if (ModInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(1.0f, OutMagnitude))
			{
				// 确保返回的是消耗的绝对值（通常GE中是负值）
				OutMagnitude = FMath::Abs(OutMagnitude);
				return true;
			}
			break;
		}
	}

	return false;
}

bool ATacticHUD::PredictResourceCost(ATacticBaseCharacter* SourceCharacter, UBaseAbility* BaseAbility,
                                     TSubclassOf<UGameplayModMagnitudeCalculation> CalculationClass, float& OutCost)
{
	if (!SourceCharacter || !BaseAbility || !CalculationClass) return false;

	// 创建计算类实例
	UGameplayModMagnitudeCalculation* Calculator = NewObject<UGameplayModMagnitudeCalculation>(
		GetTransientPackage(), CalculationClass);
	if (!Calculator) return false;

	// 创建一个临时的GameplayEffectSpec
	UMyAbilityComp* SourceASC = SourceCharacter->GetMyAbilityComp();
	if (!SourceASC) return false;

	UGameplayEffect* CostEffect = BaseAbility->GetCostGameplayEffect();
	if (!CostEffect) return false;

	// 创建一个临时的GameplayEffectSpec用于计算
	FGameplayEffectSpec TempSpec(CostEffect, FGameplayEffectContextHandle(), /*todo level*/1.0f);

	// 设置源和目标
	TempSpec.GetContext().AddInstigator(SourceCharacter, SourceCharacter);

	// 如果是行动点计算
	/*if (CalculationClass->IsChildOf(UCostActionValueCalculation::StaticClass()))
	{
		float BaseCost = 1.0f; // 默认值
		TempSpec.SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.ActionCost"), BaseCost);
	}*/

	// 捕获源和目标的Tags
	SourceASC->GetOwnedGameplayTags(TempSpec.CapturedSourceTags.GetSpecTags());

	// 调用计算类的计算方法
	float CalculatedMagnitude = Calculator->CalculateBaseMagnitude_Implementation(TempSpec);

	// 通常消耗是负值，我们取绝对值
	OutCost = FMath::Abs(CalculatedMagnitude);

	return true;
}
