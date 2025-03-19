// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseAbility.generated.h"

class UBaseEffect;
class ATacticPlayerController;
class ABaseCharacter;

UENUM()
enum EAttackRangeType : uint8
{
	EAR_Circle,
	EAR_Box,
	EAR_Segment,
};

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UBaseAbility : public UGameplayAbility
{
public:

	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup", meta=(AllowPrivateAccess=true))
	TEnumAsByte<EAttackRangeType> SkillRangeType = EAR_Circle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AbilitySetup)
	bool bFocusOnMouse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AbilitySetup)
	bool bIsRangeAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup",
		meta=(AllowPrivateAccess=true, ToolTip="技能选择范围。"))
	float TargetSelectionRange = 300.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup", meta=(
		AllowPrivateAccess=true,
		ToolTip="这是第二次远程技能的范围。",
		EditCondition = "bFocusOnMouse && bIsRangeAttack",
		EditConditionHides
	))
	float SecondaryRangedRange = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="TargetSelection", meta=(ToolTip="是否包含自身作为目标"))
	bool bIncludeSelf = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="TargetSelection", meta=(ToolTip="是否无视距离限制"))
	bool bInfiniteRange = false;

protected:
	UPROPERTY(BlueprintReadOnly, Category = AbilitySetup)
	ABaseCharacter* BaseCharacterOwner;

	GENERATED_BODY()
};
