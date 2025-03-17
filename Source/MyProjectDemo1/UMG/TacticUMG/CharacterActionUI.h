// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterActionUI.generated.h"

class UTacticSubsystem;
class UActionButtonUI;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UCharacterActionUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UActionButtonUI* ActionButton_Attack;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UActionButtonUI* ActionButton_Skill;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UActionButtonUI* ActionButton_Move;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UActionButtonUI* ActionButton_SwitchGameModeTest;
protected:
	UPROPERTY()
	UTacticSubsystem* TacticSubsystem;
	virtual void NativeConstruct() override;
};
