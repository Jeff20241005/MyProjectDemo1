// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MyProjectDemo1/Subsystems/TacticSubsystem.h"
#include "BaseAbility.generated.h"

class UBaseEffect;
class ATacticPlayerController;
class ABaseCharacter;
class AVisualFeedbackActor;

UENUM()
enum EAttackRangeType : uint8
{
	EAR_Circle UMETA(DisplayName = "Circle", ToolTip = "圆形范围效果"),
	EAR_Box UMETA(DisplayName = "Box", ToolTip = "矩形范围效果"),
	EAR_Sector UMETA(DisplayName = "Sector", ToolTip = "扇形范围效果"),
	EAR_Cross UMETA(DisplayName = "Cross", ToolTip = "十字形范围效果"),
};

/**
 * 基础技能类，提供技能释放、目标选择和效果应用的功能
 */
UCLASS()
class MYPROJECTDEMO1_API UBaseAbility : public UGameplayAbility
{
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Targeting")
	bool GetPotentialTargets(
		UTacticSubsystem* InTacticSubsystem, const FVector& TargetLocation, bool bAddMovingRange = false);

	UFUNCTION(BlueprintCallable, Category = "Ability|Targeting")

	void SelectTargetsByTeamAndProperties(UTacticSubsystem* InTacticSubsystem, ABaseCharacter* Owner_Caster,
	                                      TArray<ABaseCharacter*>& PotentialTargets) const;

	UFUNCTION(BlueprintCallable, Category = "Ability|Targeting")
	TArray<ABaseCharacter*> GetTargetsInMaxRange(ABaseCharacter* InOwner, UTacticSubsystem* InTacticSubsystem);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup",
		meta=( ToolTip="技能显示的名称"))
	FText SkillName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup",
		meta=( ToolTip="技能的释放时，对目标产生的效果"))
	TSubclassOf<UBaseEffect> BaseEffect;

	/**
	 * 获取技能可选择的目标
	 * @param OutTargets 输出参数，存储找到的目标角色
	 * @param SourceCharacter 可选参数，技能的施放者
	 * @param InTacticSubsystem
	 * @param TargetLocation
	 * @return 如果鼠标位置在有效范围内返回true，否则返回false
	 */

	/** 技能范围类型，决定技能的作用形状 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup",
		meta=(AllowPrivateAccess=true, ToolTip="技能的作用范围形状，影响目标选择和视觉反馈"))
	TEnumAsByte<EAttackRangeType> SkillRangeType = EAR_Circle;

	/** 是否为负面效果（伤害、减益等） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AbilitySetup,
		meta=(ToolTip="技能是否为负面效果，影响目标选择逻辑（敌对/友好）"))
	bool bIsNegativeEffect = true;


	/** 是否使用鼠标指向来确定技能释放位置 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AbilitySetup,
		meta=(ToolTip="是否使用鼠标位置来确定技能释放点，而不是直接以施法者为中心"))
	bool bAimWithMouse;

	/** 技能释放位置调整半径 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup", meta=(
		AllowPrivateAccess=true,
		ToolTip="鼠标引导模式下，以角色为中心的技能释放位置调整半径（单位：厘米），在此范围内可自由移动技能施放点",
		EditCondition = "bAimWithMouse",
		EditConditionHides
	))
	float SkillPlacementRadius = 0.0f;

	/** 技能是否朝向鼠标位置 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup", meta=(ToolTip=
		"技能是否看向鼠标位置（玩家可自由选择角度），注意：只旋转Yaw。当bAimWithMouse开启时，这个自动为false",
		EditCondition = "!bAimWithMouse",
		EditConditionHides))
	bool bSkillLookAtMouseHoveringLocation = false;

	/** 是否包含自身作为目标 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup", meta=(ToolTip="是否将施法者自身包含在目标列表中"))
	bool bIncludeSelf = false;

	/** 是否无视距离限制 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup", meta=(ToolTip="是否无视距离限制，适用于全图技能"))
	bool bInfiniteRange = false;

	/** 技能作用范围 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup",
		meta=(AllowPrivateAccess=true, ToolTip="技能生效的范围大小（单位：厘米），影响攻击/治疗/Buff等效果的作用范围"))
	float CircleTargetingRange = 300.0f;

	/** 扇形技能的角度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup", meta=(
		AllowPrivateAccess=true,
		ToolTip="扇形技能的角度范围（度），仅当SkillRangeType为扇形时有效",
		EditCondition = "SkillRangeType == EAR_Sector",
		ClampMin = "1.0",
		ClampMax = "360.0",
		UIMin = "1.0",
		UIMax = "360.0",
		EditConditionHides
	))
	float SectorAngle = 90.0f;

	/** 矩形技能的宽度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup", meta=(
		AllowPrivateAccess=true,
		ToolTip="矩形技能的宽度（单位：厘米），仅当SkillRangeType为矩形时有效",
		EditCondition = "SkillRangeType == EAR_Box",
		ClampMin = "1.0",
		EditConditionHides
	))
	float BoxWidth = 200.0f;

	/** 矩形技能的长度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup", meta=(
		AllowPrivateAccess=true,
		ToolTip="矩形技能的长度（单位：厘米），仅当SkillRangeType为矩形时有效",
		EditCondition = "SkillRangeType == EAR_Box",
		ClampMin = "1.0",
		EditConditionHides
	))
	float BoxLength = 400.0f;

	/** 十字技能的宽度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup", meta=(
		AllowPrivateAccess=true,
		ToolTip="十字技能的宽度（单位：厘米），仅当SkillRangeType为十字时有效",
		EditCondition = "SkillRangeType == EAR_Cross",
		ClampMin = "1.0",
		EditConditionHides
	))
	float CrossWidth = 100.0f;

	/** 十字技能的长度 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup", meta=(
		AllowPrivateAccess=true,
		ToolTip="十字技能的长度（单位：厘米），仅当SkillRangeType为十字时有效",
		EditCondition = "SkillRangeType == EAR_Cross",
		ClampMin = "1.0",
		EditConditionHides
	))
	float CrossLength = 500.0f;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;


	UPROPERTY(BlueprintReadOnly, Category = AbilitySetup)
	ABaseCharacter* BaseCharacterOwner;

	/**
	 * 检查角色是否在圆形范围内
	 */
	bool IsCharacterInCircleRange(const FVector& Center, ABaseCharacter* Character, float UseCustomRadius = -1.f) const;

	/**
	 * 检查角色是否在矩形范围内
	 */
	bool IsCharacterInBoxRange(const FVector& Center, const FVector& Forward, ABaseCharacter* Character) const;

	/**
	 * 检查角色是否在扇形范围内
	 */
	bool IsCharacterInSectorRange(const FVector& Center, const FVector& Forward, ABaseCharacter* Character) const;

	/**
	 * 检查角色是否在十字形范围内
	 */
	bool IsCharacterInCrossRange(const FVector& Center, const FVector& Forward, ABaseCharacter* Character) const;

	GENERATED_BODY()
};
