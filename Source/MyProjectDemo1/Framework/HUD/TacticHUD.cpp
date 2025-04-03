// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticHUD.h"

#include "GameplayEffect.h"
#include "MyProjectDemo1/Characters/TacticBaseCharacter.h"
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

template<typename T>
bool ATacticHUD::CheckResourceForAttribute(ATacticBaseCharacter* SourceCharacter, 
                                          UBaseAbility* BaseAbility,
                                          const FGameplayAttribute& Attribute, 
                                          T (UBaseCharacterAttributeSet::*GetValueFunc)() const,
                                          float& OutResourceCost)
{
    // 获取当前资源值
    float SourceValue = (SourceCharacter->GetBaseCharacterAttributeSet()->*GetValueFunc)();
    OutResourceCost = 0.0f;
    bool bFoundModifier = false;
    
    // 从GameplayEffect中获取Modifier
    UGameplayEffect* CostGE = BaseAbility->GetCostGameplayEffect();
    if (!CostGE) return true; // 如果没有消耗效果，默认资源足够
    
    for (const FGameplayModifierInfo& ModInfo : CostGE->Modifiers)
    {
        // 检查是否是修改指定属性的Modifier
        if (ModInfo.Attribute == Attribute)
        {
            bFoundModifier = true;
            
            // 检查修改类型
            if (ModInfo.ModifierOp != EGameplayModOp::Additive)
            {
                FString TempStr = FString::Printf(TEXT("资源消耗必须使用加法修改器 (Additive)"));
                if (GEngine)
                    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(2, 2));
                UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
                continue;
            }
            
            // 根据不同的计算类型获取消耗值
            switch (ModInfo.ModifierMagnitude.GetMagnitudeCalculationType())
            {
            case EGameplayEffectMagnitudeCalculation::ScalableFloat:
                {
                    float Cost = 0.0f;
                    // 获取修改值（通常是负数，表示消耗）
                    if (ModInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(BaseAbility->GetAbilityLevel(), Cost))
                    {
                        OutResourceCost = FMath::Abs(Cost); // 取绝对值，因为消耗是负数
                    }
                }
                break;
                
            case EGameplayEffectMagnitudeCalculation::AttributeBased:
            case EGameplayEffectMagnitudeCalculation::CustomCalculationClass:
            case EGameplayEffectMagnitudeCalculation::SetByCaller:
                {
                    FString TempStr = FString::Printf(TEXT("暂不支持的消耗计算类型"));
                    if (GEngine)
                        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TempStr, true, FVector2D(2, 2));
                    UE_LOG(LogTemp, Warning, TEXT("%s"), *TempStr);
                }
                break;
            }
            
            // 找到了对应的Modifier，不需要继续循环
            break;
        }
    }
    
    // 如果没有找到对应的Modifier，则认为不需要消耗这种资源
    if (!bFoundModifier) return true;
    
    // 检查资源是否足够
    return SourceValue >= OutResourceCost;
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
            // Todo: 行动点UI变化
        }
        
        if (!bHasEnoughMana)
        {
            if (!Message.IsEmpty()) Message.Append(TEXT("\n"));
            Message.Append(FString::Printf(TEXT("魔法值不足! 需要: %.0f, 当前: %.0f"), 
                ManaCost, SourceCharacter->GetBaseCharacterAttributeSet()->GetMana()));
                
            // 这里可以添加UI变化代码，比如闪烁魔法值显示等
            // Todo: 魔法值UI变化
        }
        
        if (GEngine && !Message.IsEmpty())
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, Message, true, FVector2D(1.5, 1.5));
    }
    
    // 只有当两种资源都足够时才返回true
    return bHasEnoughAction && bHasEnoughMana;
}
