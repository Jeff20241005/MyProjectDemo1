// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionButtonUI.generated.h"

class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UActionButtonUI : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UTextBlock* TextBlock;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Button;
};
