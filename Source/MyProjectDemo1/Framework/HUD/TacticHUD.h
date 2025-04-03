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

protected:
	/**
	 * 检查特定属性资源是否足够
	 * @param SourceCharacter 角色
	 * @param BaseAbility 技能
	 * @param Attribute 要检查的属性
	 * @param GetValueFunc 获取当前值的函数指针
	 * @param OutResourceCost 输出参数，返回消耗值
	 * @return 是否有足够的资源
	 */
	template<typename T>
	bool CheckResourceForAttribute(ATacticBaseCharacter* SourceCharacter, 
								  UBaseAbility* BaseAbility,
								  const FGameplayAttribute& Attribute, 
								  T (UBaseCharacterAttributeSet::*GetValueFunc)() const,
								  float& OutResourceCost);
};
