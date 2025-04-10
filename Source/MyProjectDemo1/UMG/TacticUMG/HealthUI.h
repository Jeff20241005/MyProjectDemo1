// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthUI.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UHealthUI : public UUserWidget
{
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UProgressBar* ProgressBar_Health;
	
	UFUNCTION(BlueprintNativeEvent)
	void ChangeHealthPercent(float Health,float MaxHealth);
	GENERATED_BODY()
};
