// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionBase.h"
#include "ActionCheckBoxUI.generated.h"

class UCheckBox;
/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UActionCheckBoxUI : public UActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UCheckBox* CheckBox;
	
	/** 切换复选框状态（选中/未选中） */
	UFUNCTION(BlueprintCallable, Category="UI|Checkbox")
	void ToggleCheckBox();
	
	/** 设置复选框状态 */
	UFUNCTION(BlueprintCallable, Category="UI|Checkbox")
	void SetIsChecked(bool bIsChecked);
	
	/** 获取复选框是否选中 */
	UFUNCTION(BlueprintPure, Category="UI|Checkbox")
	bool IsChecked() const;
};
