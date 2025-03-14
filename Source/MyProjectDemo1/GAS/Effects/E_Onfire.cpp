// Fill out your copyright notice in the Description page of Project Settings.


#include "E_Onfire.h"

#include "MyProjectDemo1/GAS/Attributes/BaseCharacterAttributeSet.h"

UE_Onfire::UE_Onfire()
{
	// 初始化效果为即时生效
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// 设置最大生命值增加的属性
	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.Attribute = UBaseCharacterAttributeSet::GetMaxHealthAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.ModifierMagnitude = FScalableFloat(100.0f);  // 增加 100 生命值

	Modifiers.Add(ModifierInfo);

	// 设置效果的唯一标识符（例如技能 ID 或物品类型）
	EffectID = "HealthBoostEffect";

	// 可以添加更多的属性修改
	FGameplayModifierInfo HealthRegenerationInfo;
	HealthRegenerationInfo.Attribute = UBaseCharacterAttributeSet::GetHealthAttribute();
	HealthRegenerationInfo.ModifierOp = EGameplayModOp::Multiplicitive;
	HealthRegenerationInfo.ModifierMagnitude = FScalableFloat(1.2f);  // 生命恢复速度增加 20%
	Modifiers.Add(HealthRegenerationInfo);


	
}

void UE_Onfire::OnGameplayEffectChanged()
{
	{
		FString
			TempStr = FString::Printf(TEXT("Fire EffectChanged  Called"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}

	Super::OnGameplayEffectChanged();
}