// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseHUD.h"
#include "TacticHUD.generated.h"

class UTacticMainUI;
class UTacticSubsystem;

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API ATacticHUD : public ABaseHUD
{
	GENERATED_BODY()
	virtual void BeginPlay() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	TSubclassOf<UTacticMainUI> MainUIClass;
	UPROPERTY()
	UTacticMainUI* MainUI;
	
	/** 切换自动移动选项 */
	UFUNCTION(BlueprintCallable, Category="Tactic|UI")
	void ToggleAutomaticMoveBySkill();
};
