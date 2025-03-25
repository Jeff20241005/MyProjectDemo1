// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillUI.generated.h"

class UBaseAbility;
class UActionButtonUI;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API USkillUI : public UUserWidget
{

public:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UActionButtonUI* ActionButtonUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UBaseAbility* BaseAbility;

protected:
	UFUNCTION()
	void ActionButtonUIOnHovered();
	UFUNCTION()
	void ActionButtonUIOnClicked();
	UFUNCTION()
	void ActionButtonUIOnUnhovered();
	virtual void NativeConstruct() override;
	GENERATED_BODY()
};
