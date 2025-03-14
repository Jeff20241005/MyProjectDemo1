// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEffect.h"
#include "MyProjectDemo1/GAS/Executions/BaseDamageExecution.h"

UBaseEffect::UBaseEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	TArray<FGameplayEffectExecutionDefinition>::ElementType Element;
	Element.CalculationClass = UBaseDamageExecution::StaticClass();
	Executions.Add(Element);
}
