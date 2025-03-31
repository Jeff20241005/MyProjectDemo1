// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyProjectDemo1/Framework/Controllers/TacticPlayerController.h"
#include "CharacterActionUI.generated.h"

class USkillListUI;
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
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UActionButtonUI* ActionButton_SwitchCharacter;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	USkillListUI* SkillListUI;

protected:
	UPROPERTY()
	UTacticSubsystem* TacticSubsystem;
	UFUNCTION()
	void ActionButton_SkillFunction();
	UFUNCTION()
	void TestSwitchCharac();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_PreMove();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_CancelMove();
	
	void PreMove(ATacticPlayerController* TacticPlayerController, UBaseAbility* BaseAbility);
	void CancelPreMove();
	UFUNCTION()
	void ActionButton_MoveOnClick();
	virtual void NativeConstruct() override;
};
