// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "PassiveAbility.generated.h"


/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UPassiveAbility : public UBaseAbility
{
	/**
	 * 覆写激活技能函数
	 * @param Handle 技能实例的句柄
	 * @param ActorInfo 技能拥有者信息
	 * @param ActivationInfo 激活信息
	 * @param TriggerEventData 游戏事件信息
	 */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData* TriggerEventData) override;
	
	/**
	 * 接收到技能结束回调函数
	 * @param AbilityTag 结束的技能标识标签
	 */
	void OnDeactivatePassiveAbility(const FGameplayTag& AbilityTag);

	virtual void BeginDestroy() override;
	GENERATED_BODY()
};
