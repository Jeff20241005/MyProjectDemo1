// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterAttributeSet.h"
#include "MonsterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UMonsterAttributeSet : public UBaseCharacterAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FGameplayAttributeData RewardValue = 1.f;
	ATTRIBUTE_ACCESSORS(ThisClass, RewardValue)
};
