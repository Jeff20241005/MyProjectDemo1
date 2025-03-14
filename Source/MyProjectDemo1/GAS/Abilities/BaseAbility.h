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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	bool bFocusOnMouse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	bool bIsRangeAttack;
	//Tag，是首先指定的，是源头
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup")
	float AttackRange = 300.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AbilitySetup", meta=(AllowPrivateAccess=true))
	float SubAttackRange = 0.0f;

protected:
	UPROPERTY(BlueprintReadOnly, Category = JFSetting)
	ABaseCharacter* BaseCharacterOwner;

	GENERATED_BODY()
};
