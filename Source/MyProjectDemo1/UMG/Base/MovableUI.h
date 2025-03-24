// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MovableUI.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UMovableUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Button_Close;

	
};
