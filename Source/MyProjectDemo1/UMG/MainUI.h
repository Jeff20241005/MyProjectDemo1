// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

class UCharacterActionUI;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UMainUI : public UUserWidget
{
public:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UCharacterActionUI* CharacterActionUI;
	GENERATED_BODY()
};
