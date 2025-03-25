// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillListUI.generated.h"

class UScrollBox;
class USkillUI;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API USkillListUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)	
	UScrollBox* ScrollBox;
	void GenerateList(APlayerCharacter* PlayerCharacter);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	TSubclassOf<USkillUI> SkillUIClass;

protected:
};
