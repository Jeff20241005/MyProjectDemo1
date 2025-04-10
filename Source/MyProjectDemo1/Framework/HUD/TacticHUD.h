// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseHUD.h"
#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"
#include "TacticHUD.generated.h"

class UTacticMainUI;
class UTacticSubsystem;
class UGameplayEffect;
class UBaseAbility;
class UGameplayModMagnitudeCalculation;

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API ATacticHUD : public ABaseHUD
{
	GENERATED_BODY()
	virtual void BeginPlay() override;
		
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	TSubclassOf<UTacticMainUI> MainUIClass;
	UPROPERTY()
	UTacticMainUI* MainUI;
	
	/** 切换自动移动选项 */
	UFUNCTION(BlueprintCallable, Category="Tactic|UI")
	void ToggleAutomaticMoveBySkill();

	
	/**
	 * 检查是否有足够的资源使用此技能，如果没有，比如行动点缺乏
	 * 就给行动点的UI做一些变化，
	 * @param SourceCharacter 检查的角色
	 * @param BaseAbility 使用的技能
	 * @return 是否有足够的资源使用此技能
	 */
	bool CheckHasEnoughResources(ATacticBaseCharacter* SourceCharacter, UBaseAbility* BaseAbility);

	/**
	 * 预测技能的资源消耗值
	 * @param SourceCharacter 技能施放者
	 * @param BaseAbility 要使用的技能
	 * @param CalculationClass 用于计算消耗的计算类
	 * @param OutCost 输出参数，计算得到的消耗值
	 * @return 是否成功计算出消耗值
	 */
	bool PredictResourceCost(ATacticBaseCharacter* SourceCharacter, UBaseAbility* BaseAbility, 
	                         TSubclassOf<UGameplayModMagnitudeCalculation> CalculationClass, float& OutCost);

	/**
	 * 从Gameplay Effect中获取修改特定属性的Modifier的幅度
	 * @param GameplayEffect 要检查的效果
	 * @param Attribute 要寻找的属性
	 * @param OutMagnitude 输出参数，找到的幅度值
	 * @return 是否找到对应的Modifier
	 */
	bool GetModifierMagnitudeForAttribute(const UGameplayEffect* GameplayEffect, const FGameplayAttribute& Attribute, float& OutMagnitude);

protected:
	/**
	 * 检查角色是否有足够的特定资源
	 * @param Character 要检查的角色
	 * @param Ability 使用的技能
	 * @param Attribute 资源属性
	 * @param GetterFunc 获取当前资源值的函数
	 * @param OutCost 输出参数，计算得到的消耗值
	 * @return 是否有足够的资源
	 */
	template<class T>
	bool CheckResourceForAttribute(ATacticBaseCharacter* Character, UBaseAbility* Ability,
		const FGameplayAttribute& Attribute, T GetterFunc, float& OutCost);
	
};
