// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "BaseDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UBaseDamageExecution : public UGameplayEffectExecutionCalculation
{
	UBaseDamageExecution();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
	GENERATED_BODY()
};
